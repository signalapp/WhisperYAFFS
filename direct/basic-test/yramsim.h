// NAND RAM Simulator for testing YAFFS
#ifndef __YAFFS_RAM_SIM_H__
#define __YAFFS_RAM_SIM_H__


#include "yaffs_guts.h"

#define N_RAM_SIM_DEVS  2

struct yaffs_DeviceStruct *yramsim_CreateSim(const YCHAR *name,
						__u32 devId, __u32 nBlocks,
						__u32 startBlock, __u32 endBlock);

#endif
