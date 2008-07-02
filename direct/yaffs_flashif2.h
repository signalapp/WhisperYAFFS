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

#ifndef __YAFFS_FLASH2_H__
#define __YAFFS_FLASH2_H__


#include "yaffs_guts.h"
int yflash2_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int yflash2_WriteChunkToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_Spare *spare);
int yflash2_WriteChunkWithTagsToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_ExtendedTags *tags);
int yflash2_ReadChunkFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare);
int yflash2_ReadChunkWithTagsFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_ExtendedTags *tags);
int yflash2_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int yflash2_InitialiseNAND(yaffs_Device *dev);
int yflash2_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockNo);
int yflash2_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockNo, yaffs_BlockState *state, __u32 *sequenceNumber);

#endif
