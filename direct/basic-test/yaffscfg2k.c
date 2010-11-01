/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * yaffscfg2k.c  The configuration for the "direct" use of yaffs.
 *
 * This file is intended to be modified to your requirements.
 * There is no need to redistribute this file.
 */

#include "yaffscfg.h"
#include "yaffs_guts.h"
#include "yaffsfs.h"
#include "yaffs_fileem2k.h"
#include "yaffs_nandemul2k.h"
#include "yaffs_norif1.h"
#include "yaffs_trace.h"


#include <errno.h>

unsigned yaffs_trace_mask = 

	YAFFS_TRACE_SCAN |  
	YAFFS_TRACE_GC |
	YAFFS_TRACE_ERASE | 
	YAFFS_TRACE_ERROR | 
	YAFFS_TRACE_TRACING | 
	YAFFS_TRACE_ALLOCATE | 
	YAFFS_TRACE_BAD_BLOCKS |
	YAFFS_TRACE_VERIFY | 
	
	0;
        

static int yaffsfs_lastError;

void yaffsfs_SetError(int err)
{
	//Do whatever to set error
	yaffsfs_lastError = err;
}


int yaffsfs_GetLastError(void)
{
	return yaffsfs_lastError;
}

void yaffsfs_Lock(void)
{
}

void yaffsfs_Unlock(void)
{
}

u32 yaffsfs_CurrentTime(void)
{
	return 0;
}


static int yaffs_kill_alloc = 0;
static size_t total_malloced = 0;
static size_t malloc_limit = 0 & 6000000;

void *yaffs_malloc(size_t size)
{
	void * this;
	if(yaffs_kill_alloc)
		return NULL;
	if(malloc_limit && malloc_limit <(total_malloced + size) )
		return NULL;

	this = malloc(size);
	if(this)
		total_malloced += size;
	return this;
}

void yaffs_free(void *ptr)
{
	free(ptr);
}

void yaffsfs_LocalInitialisation(void)
{
	// Define locking semaphore.
}

// Configuration

#include "yaffs_ramdisk.h"
#include "yaffs_flashif.h"
#include "yaffs_flashif2.h"
#include "yaffs_nandemul2k.h"

struct yaffs_dev ram1Dev;
struct yaffs_dev flashDev;
struct yaffs_dev m18_1Dev;

int yaffs_start_up(void)
{
	// Stuff to configure YAFFS
	// Stuff to initialise anything special (eg lock semaphore).
	yaffsfs_LocalInitialisation();
	
	// Set up devices
	// /ram1   ram, yaffs1
	memset(&ram1Dev,0,sizeof(ram1Dev));
	ram1Dev.param.name = "ram1";
	ram1Dev.param.total_bytes_per_chunk = 512;
	ram1Dev.param.chunks_per_block = 32;
	ram1Dev.param.n_reserved_blocks = 2; // Set this smaller for RAM
	ram1Dev.param.start_block = 0; // Can use block 0
	ram1Dev.param.end_block = 127; // Last block in 2MB.	
	//ram1Dev.param.use_nand_ecc = 1;
	ram1Dev.param.n_caches = 0;	// Disable caching on this device.
	ram1Dev.driver_context = (void *) 0;	// Used to identify the device in fstat.
	ram1Dev.param.write_chunk_tags_fn = yramdisk_wr_chunk;
	ram1Dev.param.read_chunk_tags_fn = yramdisk_rd_chunk;
	ram1Dev.param.erase_fn = yramdisk_erase;
	ram1Dev.param.initialise_flash_fn = yramdisk_initialise;
	
	yaffs_add_device(&ram1Dev);

	// /M18-1 yaffs1 on M18 nor sim
	memset(&m18_1Dev,0,sizeof(m18_1Dev));
	m18_1Dev.param.name = "M18-1";
	m18_1Dev.param.total_bytes_per_chunk = 1024;
	m18_1Dev.param.chunks_per_block =248;
	m18_1Dev.param.n_reserved_blocks = 2;
	m18_1Dev.param.start_block = 0; // Can use block 0
	m18_1Dev.param.end_block = 31; // Last block
	m18_1Dev.param.use_nand_ecc = 0; // use YAFFS's ECC
	m18_1Dev.param.n_caches = 10; // Use caches
	m18_1Dev.driver_context = (void *) 1;	// Used to identify the device in fstat.
	m18_1Dev.param.write_chunk_fn = ynorif1_WriteChunkToNAND;
	m18_1Dev.param.read_chunk_fn = ynorif1_ReadChunkFromNAND;
	m18_1Dev.param.erase_fn = ynorif1_EraseBlockInNAND;
	m18_1Dev.param.initialise_flash_fn = ynorif1_InitialiseNAND;
	m18_1Dev.param.deinitialise_flash_fn = ynorif1_Deinitialise_flash_fn;

//	m18_1Dev.param.disable_soft_del = 1;

	yaffs_add_device(&m18_1Dev);

	// /yaffs2  yaffs2 file emulation
	// 2kpage/64chunk per block
	//
	memset(&flashDev,0,sizeof(flashDev));
	flashDev.param.name = "yaffs2";
	flashDev.param.total_bytes_per_chunk = 2048;
	flashDev.param.chunks_per_block = 64;
	flashDev.param.n_reserved_blocks = 5;
	flashDev.param.inband_tags = 0;
	flashDev.param.start_block = 0;
	flashDev.param.end_block = yflash2_GetNumberOfBlocks()-1;
	flashDev.param.is_yaffs2 = 1;
	flashDev.param.use_nand_ecc=1;
	flashDev.param.wide_tnodes_disabled=0;
	flashDev.param.refresh_period = 1000;
	flashDev.param.n_caches = 10; // Use caches
	flashDev.driver_context = (void *) 2;	// Used to identify the device in fstat.
	flashDev.param.write_chunk_tags_fn = yflash2_WriteChunkWithTagsToNAND;
	flashDev.param.read_chunk_tags_fn = yflash2_ReadChunkWithTagsFromNAND;
	flashDev.param.erase_fn = yflash2_EraseBlockInNAND;
	flashDev.param.initialise_flash_fn = yflash2_InitialiseNAND;
	flashDev.param.bad_block_fn = yflash2_MarkNANDBlockBad;
	flashDev.param.query_block_fn = yflash2_QueryNANDBlock;
	flashDev.param.enable_xattr = 1;

	yaffs_add_device(&flashDev);

// todo	yaffs_initialise(yaffsfs_config);
	
	return 0;
}



