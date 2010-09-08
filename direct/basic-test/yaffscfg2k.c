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

unsigned yaffs_traceMask = 

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

__u32 yaffsfs_CurrentTime(void)
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

struct yaffs_DeviceStruct ram1Dev;
struct yaffs_DeviceStruct flashDev;
struct yaffs_DeviceStruct m18_1Dev;

int yaffs_StartUp(void)
{
	// Stuff to configure YAFFS
	// Stuff to initialise anything special (eg lock semaphore).
	yaffsfs_LocalInitialisation();
	
	// Set up devices
	// /ram1   ram, yaffs1
	memset(&ram1Dev,0,sizeof(ram1Dev));
	ram1Dev.param.name = "ram1";
	ram1Dev.param.totalBytesPerChunk = 512;
	ram1Dev.param.nChunksPerBlock = 32;
	ram1Dev.param.nReservedBlocks = 2; // Set this smaller for RAM
	ram1Dev.param.startBlock = 0; // Can use block 0
	ram1Dev.param.endBlock = 127; // Last block in 2MB.	
	//ram1Dev.param.useNANDECC = 1;
	ram1Dev.param.nShortOpCaches = 0;	// Disable caching on this device.
	ram1Dev.driverContext = (void *) 0;	// Used to identify the device in fstat.
	ram1Dev.param.writeChunkWithTagsToNAND = yramdisk_WriteChunkWithTagsToNAND;
	ram1Dev.param.readChunkWithTagsFromNAND = yramdisk_ReadChunkWithTagsFromNAND;
	ram1Dev.param.eraseBlockInNAND = yramdisk_EraseBlockInNAND;
	ram1Dev.param.initialiseNAND = yramdisk_InitialiseNAND;
	
	yaffs_AddDevice(&ram1Dev);

	// /M18-1 yaffs1 on M18 nor sim
	memset(&m18_1Dev,0,sizeof(m18_1Dev));
	m18_1Dev.param.name = "M18-1";
	m18_1Dev.param.totalBytesPerChunk = 1024;
	m18_1Dev.param.nChunksPerBlock =248;
	m18_1Dev.param.nReservedBlocks = 2;
	m18_1Dev.param.startBlock = 0; // Can use block 0
	m18_1Dev.param.endBlock = 31; // Last block
	m18_1Dev.param.useNANDECC = 0; // use YAFFS's ECC
	m18_1Dev.param.nShortOpCaches = 10; // Use caches
	m18_1Dev.driverContext = (void *) 1;	// Used to identify the device in fstat.
	m18_1Dev.param.writeChunkToNAND = ynorif1_WriteChunkToNAND;
	m18_1Dev.param.readChunkFromNAND = ynorif1_ReadChunkFromNAND;
	m18_1Dev.param.eraseBlockInNAND = ynorif1_EraseBlockInNAND;
	m18_1Dev.param.initialiseNAND = ynorif1_InitialiseNAND;
	m18_1Dev.param.deinitialiseNAND = ynorif1_DeinitialiseNAND;

//	m18_1Dev.param.disableSoftDelete = 1;

	yaffs_AddDevice(&m18_1Dev);

	// /yaffs2  yaffs2 file emulation
	// 2kpage/64chunk per block
	//
	memset(&flashDev,0,sizeof(flashDev));
	flashDev.param.name = "yaffs2";
	flashDev.param.totalBytesPerChunk = 2048;
	flashDev.param.nChunksPerBlock = 64;
	flashDev.param.nReservedBlocks = 5;
	flashDev.param.inbandTags = 0;
	flashDev.param.startBlock = 0;
	flashDev.param.endBlock = yflash2_GetNumberOfBlocks()-1;
	flashDev.param.isYaffs2 = 1;
	flashDev.param.useNANDECC=1;
	flashDev.param.wideTnodesDisabled=0;
	flashDev.param.refreshPeriod = 1000;
	flashDev.param.nShortOpCaches = 10; // Use caches
	flashDev.driverContext = (void *) 2;	// Used to identify the device in fstat.
	flashDev.param.writeChunkWithTagsToNAND = yflash2_WriteChunkWithTagsToNAND;
	flashDev.param.readChunkWithTagsFromNAND = yflash2_ReadChunkWithTagsFromNAND;
	flashDev.param.eraseBlockInNAND = yflash2_EraseBlockInNAND;
	flashDev.param.initialiseNAND = yflash2_InitialiseNAND;
	flashDev.param.markNANDBlockBad = yflash2_MarkNANDBlockBad;
	flashDev.param.queryNANDBlock = yflash2_QueryNANDBlock;
	flashDev.param.enableXattr = 1;

	yaffs_AddDevice(&flashDev);

// todo	yaffs_initialise(yaffsfs_config);
	
	return 0;
}



