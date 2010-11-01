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


#ifndef __YAFFS_NOR_IF1_H__
#define __YAFFS_NOR_IF1_H__

#include "yaffs_guts.h"

int ynorif1_WriteChunkToNAND(yaffs_dev_t *dev,int nand_chunk,const u8 *data, const yaffs_spare *spare);
int ynorif1_ReadChunkFromNAND(yaffs_dev_t *dev,int nand_chunk, u8 *data, yaffs_spare *spare);
int ynorif1_EraseBlockInNAND(yaffs_dev_t *dev, int blockNumber);
int ynorif1_InitialiseNAND(yaffs_dev_t *dev);
int ynorif1_Deinitialise_flash_fn(yaffs_dev_t *dev);

#endif


