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

/*
 * yaffs_ramdisk.h: yaffs ram disk component
 */

#ifndef __YAFFS_RAMDISK_H__
#define __YAFFS_RAMDISK_H__


#include "yaffs_guts.h"
int yramdisk_erase(yaffs_dev_t *dev, int blockNumber);
int yramdisk_wr_chunk(yaffs_dev_t *dev,int nand_chunk,const u8 *data, const yaffs_ext_tags *tags);
int yramdisk_rd_chunk(yaffs_dev_t *dev,int nand_chunk, u8 *data, yaffs_ext_tags *tags);
int yramdisk_initialise(yaffs_dev_t *dev);
int yramdisk_mark_block_bad(yaffs_dev_t *dev,int blockNumber);
int yramdisk_query_block(yaffs_dev_t *dev, int block_no, yaffs_block_state_t *state, int *seq_number);
#endif
