/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */


#ifndef __YNANDIF_H__
#define __YNANDIF_H__

#include "yaffs_guts.h"


typedef struct {
	unsigned startBlock;
	unsigned endBlock;
	unsigned dataSize;
	unsigned spareSize;
	unsigned pagesPerBlock;
	unsigned hasECC;
	unsigned inbandTags;
	unsigned useYaffs2;

	int (*initialise)(yaffs_Device *dev);
	int (*deinitialise)(yaffs_Device *dev);

	int (*readChunk) (yaffs_Device *dev,
					  unsigned pageId, 
					  unsigned char *data, unsigned dataLength,
					  unsigned char *spare, unsigned spareLength,
					  int *eccStatus);
// ECC status is set to 0 for OK, 1 for fixed, -1 for unfixed.

	int (*writeChunk)(yaffs_Device *dev,
					  unsigned pageId, 
					  const unsigned char *data, unsigned dataLength,
					  const unsigned char *spare, unsigned spareLength);

	int (*eraseBlock)(yaffs_Device *dev, unsigned blockId);

	int (*checkBlockOk)(yaffs_Device *dev, unsigned blockId);
	int (*markBlockBad)(yaffs_Device *dev, unsigned blockId);

	void *privateData;

} ynandif_Geometry;

struct yaffs_DeviceStruct * 
	yaffs_AddDeviceFromGeometry(const YCHAR *name,
					const ynandif_Geometry *geometry);

#if 0

int ynandif_WriteChunkWithTagsToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_ExtendedTags *tags);
int ynandif_ReadChunkWithTagsFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_ExtendedTags *tags);
int ynandif_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int ynandif_InitialiseNAND(yaffs_Device *dev);
int ynandif_MarkNANDBlockBad(yaffs_Device *dev,int blockNumber);
int ynandif_QueryNANDBlock(yaffs_Device *dev, int blockNo, yaffs_BlockState *state, __u32 *sequenceNumber);
int ynandif_GetGeometry(yaffs_Device *dev, ynandif_Geometry *geometry);
#endif


#endif
