// NAND Simulator for testing YAFFS
#ifndef __YAFF_RAM_SIM_H__
#define __YAFF_RAM_SIM_H__


#include "yaffs_guts.h"

struct yaffs_DeviceStruct *yramsim_CreateSim(const YCHAR *name, int nBlocks);


#endif
