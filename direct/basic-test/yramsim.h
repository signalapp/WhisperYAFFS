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

/*  NAND RAM Simulator for testing YAFFS */

#ifndef __YAFFS_RAM_SIM_H__
#define __YAFFS_RAM_SIM_H__


#include "yaffs_guts.h"

#define N_RAM_SIM_DEVS  2

struct yaffs_DeviceStruct *yramsim_CreateRamSim(const YCHAR *name,
						__u32 devId, __u32 nBlocks,
						__u32 startBlock, __u32 endBlock);

#endif


