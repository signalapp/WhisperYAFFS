/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 *
 * yaffs_nandemul.h: Interface to emulated NAND functions
 *
 * $Id: yaffs_nandemul.h,v 1.1 2004-12-17 04:39:04 charles Exp $
 */
 
#ifndef __YAFFS_NANDEMUL_H__
#define __YAFFS_NANDEMUL_H__

#include "yaffs_guts.h" 
 
 
/* WriteChunkToNAND and ReadChunkFromNAND are used with two pointers. 
 * If either of these pointers are null, then that field will not be 
 * transferred.
 */
 
int nandemul_WriteChunkToNAND(struct yaffs_DeviceStruct *dev,int chunkInNAND, const __u8 *data, yaffs_Spare *spare);
int nandemul_ReadChunkFromNAND(struct yaffs_DeviceStruct *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare);
int nandemul_EraseBlockInNAND(struct yaffs_DeviceStruct *dev,int blockInNAND);
int nandemul_InitialiseNAND(struct yaffs_DeviceStruct *dev);

#endif

