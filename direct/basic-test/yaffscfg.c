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
 * yaffscfg.c  The configuration for the "direct" use of yaffs.
 *
 * This file is intended to be modified to your requirements.
 * There is no need to redistribute this file.
 */

#include "yaffscfg.h"
#include "yaffsfs.h"
#include <errno.h>


#include "yramsim.h"

unsigned yaffs_trace_mask = 0xFFFFFFFF;


void yaffsfs_SetError(int err)
{
	//Do whatever to set error
	errno = err;
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

void *yaffs_malloc(size_t size)
{
	return malloc(size);
}

void yaffs_free(void *ptr)
{
	free(ptr);
}

void yaffsfs_LocalInitialisation(void)
{
	// Define locking semaphore.
}

// Configuration for:
// /ram  2MB ramdisk
// /boot 2MB boot disk (flash)
// /flash 14MB flash disk (flash)
// NB Though /boot and /flash occupy the same physical device they
// are still disticnt "struct yaffs_devs. You may think of these as "partitions"
// using non-overlapping areas in the same device.
// 

#include "yaffs_ramdisk.h"
#include "yaffs_flashif.h"

static struct yaffs_dev ramDev;
static struct yaffs_dev bootDev;
static struct yaffs_dev flashDev;

static yaffsfs_DeviceConfiguration yaffsfs_config[] = {

	{ "/ram", &ramDev},
	{ "/boot", &bootDev},
	{ "/flash", &flashDev},
	{(void *)0,(void *)0}
};


int yaffs_start_up(void)
{
	// Stuff to configure YAFFS
	// Stuff to initialise anything special (eg lock semaphore).
	yaffsfs_LocalInitialisation();
	
#if 1
	yramsim_CreateSim("yaffs2");
#else
	// Set up devices

	// /ram
	ramDev.data_bytes_per_chunk = 512;
	ramDev.chunks_per_block = 32;
	ramDev.n_reserved_blocks = 2; // Set this smaller for RAM
	ramDev.start_block = 1; // Can't use block 0
	ramDev.end_block = 127; // Last block in 2MB.	
	ramDev.use_nand_ecc = 1;
	ramDev.n_caches = 0;	// Disable caching on this device.
	ramDev.genericDevice = (void *) 0;	// Used to identify the device in fstat.
	ramDev.write_chunk_tags_fn = yramdisk_wr_chunk;
	ramDev.read_chunk_tags_fn = yramdisk_rd_chunk;
	ramDev.erase_fn = yramdisk_erase;
	ramDev.initialise_flash_fn = yramdisk_initialise;

	// /boot
	bootDev.data_bytes_per_chunk = 512;
	bootDev.chunks_per_block = 32;
	bootDev.n_reserved_blocks = 5;
	bootDev.start_block = 1; // Can't use block 0
	bootDev.end_block = 127; // Last block in 2MB.	
	bootDev.use_nand_ecc = 0; // use YAFFS's ECC
	bootDev.n_caches = 10; // Use caches
	bootDev.genericDevice = (void *) 1;	// Used to identify the device in fstat.
	bootDev.write_chunk_fn = yflash_WriteChunkToNAND;
	bootDev.read_chunk_fn = yflash_ReadChunkFromNAND;
	bootDev.erase_fn = yflash_EraseBlockInNAND;
	bootDev.initialise_flash_fn = yflash_InitialiseNAND;

		// /flash
	flashDev.data_bytes_per_chunk =  512;
	flashDev.chunks_per_block = 32;
	flashDev.n_reserved_blocks = 5;
	flashDev.start_block = 128; // First block after 2MB
	flashDev.end_block = 1023; // Last block in 16MB
	flashDev.use_nand_ecc = 0; // use YAFFS's ECC
	flashDev.n_caches = 10; // Use caches
	flashDev.genericDevice = (void *) 2;	// Used to identify the device in fstat.
	flashDev.write_chunk_fn = yflash_WriteChunkToNAND;
	flashDev.read_chunk_fn = yflash_ReadChunkFromNAND;
	flashDev.erase_fn = yflash_EraseBlockInNAND;
	flashDev.initialise_flash_fn = yflash_InitialiseNAND;

	yaffs_initialise(yaffsfs_config);
#endif
	
	return 0;
}




