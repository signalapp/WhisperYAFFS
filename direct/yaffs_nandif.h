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
	unsigned start_block;
	unsigned end_block;
	unsigned dataSize;
	unsigned spareSize;
	unsigned pagesPerBlock;
	unsigned hasECC;
	unsigned inband_tags;
	unsigned useYaffs2;

	int (*initialise)(yaffs_dev_t *dev);
	int (*deinitialise)(yaffs_dev_t *dev);

	int (*readChunk) (yaffs_dev_t *dev,
					  unsigned pageId, 
					  unsigned char *data, unsigned dataLength,
					  unsigned char *spare, unsigned spareLength,
					  int *eccStatus);
// ECC status is set to 0 for OK, 1 for fixed, -1 for unfixed.

	int (*writeChunk)(yaffs_dev_t *dev,
					  unsigned pageId, 
					  const unsigned char *data, unsigned dataLength,
					  const unsigned char *spare, unsigned spareLength);

	int (*eraseBlock)(yaffs_dev_t *dev, unsigned blockId);

	int (*checkBlockOk)(yaffs_dev_t *dev, unsigned blockId);
	int (*markBlockBad)(yaffs_dev_t *dev, unsigned blockId);

	void *privateData;

} ynandif_Geometry;

struct yaffs_dev_s * 
	yaffs_add_dev_from_geometry(const YCHAR *name,
					const ynandif_Geometry *geometry);

#if 0

int ynandif_WriteChunkWithTagsToNAND(yaffs_dev_t *dev,int nand_chunk,const __u8 *data, const yaffs_ext_tags *tags);
int ynandif_ReadChunkWithTagsFromNAND(yaffs_dev_t *dev,int nand_chunk, __u8 *data, yaffs_ext_tags *tags);
int ynandif_EraseBlockInNAND(yaffs_dev_t *dev, int blockNumber);
int ynandif_InitialiseNAND(yaffs_dev_t *dev);
int ynandif_MarkNANDBlockBad(yaffs_dev_t *dev,int blockNumber);
int ynandif_QueryNANDBlock(yaffs_dev_t *dev, int block_no, yaffs_block_state_t *state, __u32 *seq_number);
int ynandif_GetGeometry(yaffs_dev_t *dev, ynandif_Geometry *geometry);
#endif


#endif
