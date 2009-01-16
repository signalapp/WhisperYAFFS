/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
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
#include "yaffsfs.h"
#include "yaffs_fileem2k.h"
#include "yaffs_nandemul2k.h"
#include "yaffs_norif1.h"

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

// Configuration for:
// /ram  2MB ramdisk
// /boot 2MB boot disk (flash)
// /flash 14MB flash disk (flash)
// NB Though /boot and /flash occupy the same physical device they
// are still disticnt "yaffs_Devices. You may think of these as "partitions"
// using non-overlapping areas in the same device.
// 

#include "yaffs_ramdisk.h"
#include "yaffs_flashif.h"
#include "yaffs_flashif2.h"
#include "yaffs_nandemul2k.h"

static yaffs_Device ram1Dev;
static yaffs_Device nand2;
static yaffs_Device flashDev;
static yaffs_Device ram2kDev;
static yaffs_Device m18_1Dev;

static yaffsfs_DeviceConfiguration yaffsfs_config[] = {

	{ "/ram1", &ram1Dev},
	{ "/M18-1", &m18_1Dev},
	{ "/yaffs2", &flashDev},
	{ "/ram2k", &ram2kDev},
	{(void *)0,(void *)0} /* Null entry to terminate list */
};


int yaffs_StartUp(void)
{
	// Stuff to configure YAFFS
	// Stuff to initialise anything special (eg lock semaphore).
	yaffsfs_LocalInitialisation();
	
	// Set up devices
	// /ram1   ram, yaffs1
	memset(&ram1Dev,0,sizeof(ram1Dev));
	ram1Dev.totalBytesPerChunk = 512;
	ram1Dev.nChunksPerBlock = 32;
	ram1Dev.nReservedBlocks = 2; // Set this smaller for RAM
	ram1Dev.startBlock = 0; // Can use block 0
	ram1Dev.endBlock = 127; // Last block in 2MB.	
	//ram1Dev.useNANDECC = 1;
	ram1Dev.nShortOpCaches = 0;	// Disable caching on this device.
	ram1Dev.genericDevice = (void *) 0;	// Used to identify the device in fstat.
	ram1Dev.writeChunkWithTagsToNAND = yramdisk_WriteChunkWithTagsToNAND;
	ram1Dev.readChunkWithTagsFromNAND = yramdisk_ReadChunkWithTagsFromNAND;
	ram1Dev.eraseBlockInNAND = yramdisk_EraseBlockInNAND;
	ram1Dev.initialiseNAND = yramdisk_InitialiseNAND;

	// /M18-1 yaffs1 on M18 nor sim
	memset(&m18_1Dev,0,sizeof(m18_1Dev));
	m18_1Dev.totalBytesPerChunk = 1024;
	m18_1Dev.nChunksPerBlock =248;
	m18_1Dev.nReservedBlocks = 2;
	m18_1Dev.startBlock = 0; // Can use block 0
	m18_1Dev.endBlock = 31; // Last block
	m18_1Dev.useNANDECC = 0; // use YAFFS's ECC
	m18_1Dev.nShortOpCaches = 10; // Use caches
	m18_1Dev.genericDevice = (void *) 1;	// Used to identify the device in fstat.
	m18_1Dev.writeChunkToNAND = ynorif1_WriteChunkToNAND;
	m18_1Dev.readChunkFromNAND = ynorif1_ReadChunkFromNAND;
	m18_1Dev.eraseBlockInNAND = ynorif1_EraseBlockInNAND;
	m18_1Dev.initialiseNAND = ynorif1_InitialiseNAND;
	m18_1Dev.deinitialiseNAND = ynorif1_DeinitialiseNAND;


	// /yaffs2 
	// Set this puppy up to use
	// the file emulation space as
	// 2kpage/64chunk per block
	//
	memset(&flashDev,0,sizeof(flashDev));

	flashDev.totalBytesPerChunk = 2048;
	flashDev.nChunksPerBlock = 64;
	flashDev.nReservedBlocks = 5;
	flashDev.inbandTags = 0;
	flashDev.startBlock = 0;
	flashDev.endBlock = yflash2_GetNumberOfBlocks()-1;
	flashDev.isYaffs2 = 1;
	flashDev.wideTnodesDisabled=0;
	flashDev.nShortOpCaches = 10; // Use caches
	flashDev.genericDevice = (void *) 2;	// Used to identify the device in fstat.
	flashDev.writeChunkWithTagsToNAND = yflash2_WriteChunkWithTagsToNAND;
	flashDev.readChunkWithTagsFromNAND = yflash2_ReadChunkWithTagsFromNAND;
	flashDev.eraseBlockInNAND = yflash2_EraseBlockInNAND;
	flashDev.initialiseNAND = yflash2_InitialiseNAND;
	flashDev.markNANDBlockBad = yflash2_MarkNANDBlockBad;
	flashDev.queryNANDBlock = yflash2_QueryNANDBlock;


	yaffs_initialise(yaffsfs_config);
	
	return 0;
}



void SetCheckpointReservedBlocks(int n)
{
//	flashDev.nCheckpointReservedBlocks = n;
}

