/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
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

#ifndef __YAFFS_FLASH_H__
#define __YAFFS_FLASH_H__


#include "yaffs_guts.h"
int yflash_EraseBlockInNAND(yaffs_dev_t *dev, int blockNumber);
int yflash_WriteChunkToNAND(yaffs_dev_t *dev,int nand_chunk,const __u8 *data, const yaffs_spare *spare);
int yflash_WriteChunkWithTagsToNAND(yaffs_dev_t *dev,int nand_chunk,const __u8 *data, const yaffs_ext_tags *tags);
int yflash_ReadChunkFromNAND(yaffs_dev_t *dev,int nand_chunk, __u8 *data, yaffs_spare *spare);
int yflash_ReadChunkWithTagsFromNAND(yaffs_dev_t *dev,int nand_chunk, __u8 *data, yaffs_ext_tags *tags);
int yflash_InitialiseNAND(yaffs_dev_t *dev);
int yflash_MarkNANDBlockBad(struct yaffs_dev_s *dev, int block_no);
int yflash_QueryNANDBlock(struct yaffs_dev_s *dev, int block_no, yaffs_block_state_t *state, __u32 *seq_number);

#endif
