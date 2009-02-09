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
		unsigned dataSize;		// Number of data bytes per page
		unsigned spareSize;		// Number of spare bytes per chunk
		unsigned pagesPerBlock;
		unsigned hasECC;
		unsigned inbandTags;	// Use inband tags on this device
		unsigned useYaffs2;
} ynandif_Geometry;

int ynandif_WriteChunkWithTagsToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_ExtendedTags *tags);
int ynandif_ReadChunkWithTagsFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_ExtendedTags *tags);
int ynandif_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int ynandif_InitialiseNAND(yaffs_Device *dev);
int ynandif_MarkNANDBlockBad(yaffs_Device *dev,int blockNumber);
int ynandif_QueryNANDBlock(yaffs_Device *dev, int blockNo, yaffs_BlockState *state, __u32 *sequenceNumber);
int ynandif_GetGeometry(yaffs_Device *dev, ynandif_Geometry *geometry);
#endif
