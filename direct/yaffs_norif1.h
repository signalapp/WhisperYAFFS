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


#ifndef __YAFFS_NOR_IF1_H__
#define __YAFFS_NOR_IF1_H__

#include "yaffs_guts.h"

int ynorif1_WriteChunkToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_Spare *spare);
int ynorif1_ReadChunkFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare);
int ynorif1_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int ynorif1_InitialiseNAND(yaffs_Device *dev);
int ynorif1_DeinitialiseNAND(yaffs_Device *dev);

#endif


