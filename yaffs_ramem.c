/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 * yaffs_ramem.c  NAND emulation on top of a chunk of RAM
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
 //yaffs_ramem.c
 // Since this creates the RAM block at start up it is pretty useless for testing the scanner.

const char *yaffs_ramem_c_version = "$Id: yaffs_ramem.c,v 1.4 2005-07-31 04:08:08 marty Exp $";

#ifndef __KERNEL__
#define CONFIG_YAFFS_RAM_ENABLED
#else
#include <linux/config.h>
#endif

#ifdef CONFIG_YAFFS_RAM_ENABLED

#include "yportenv.h"

#include "yaffs_nandemul.h"
#include "yaffs_guts.h"
#include "yaffsinterface.h"
#include "devextras.h"


#define EM_SIZE_IN_MEG 2

#define YAFFS_BLOCK_SIZE (32 * 528)
#define BLOCKS_PER_MEG ((1024*1024)/(32 * 512))
#define FILE_SIZE_IN_BLOCKS (FILE_SIZE_IN_MEG * BLOCKS_PER_MEG)
#define FILE_SIZE_IN_BYTES (FILE_SIZE_IN_BLOCKS * YAFFS_BLOCK_SIZE)



#define DEFAULT_SIZE_IN_MB 2

typedef struct 
{
	__u8 data[528]; // Data + spare
	int count[3];   // The programming count for each area of
			// the page (0..255,256..511,512..527
	int empty;      // is this empty?
} nandemul_Page;

typedef struct
{
	nandemul_Page page[32]; // The pages in the block
	__u8 damaged; 		// Is the block damaged?
	
} nandemul_Block;



typedef struct
{
	nandemul_Block **block;
	int nBlocks;
} nandemul_Device;

static nandemul_Device ned;

static int sizeInMB = DEFAULT_SIZE_IN_MB;


static void nandemul_yield(int n)
{
#ifdef __KERNEL__
	if(n > 0) schedule_timeout(n);
#endif

}


static void nandemul_ReallyEraseBlock(int blockNumber)
{
	int i;
	
	nandemul_Block *theBlock = ned.block[blockNumber];
	
	for(i = 0; i < 32; i++)
	{
		memset(theBlock->page[i].data,0xff,528);
		theBlock->page[i].count[0] = 0;
		theBlock->page[i].count[1] = 0;
		theBlock->page[i].count[2] = 0;
		theBlock->page[i].empty = 1;
		nandemul_yield(2);
	}

}


static int nandemul_CalcNBlocks(void)
{
        switch(sizeInMB)
        {
        	case 8:
        	case 16:
        	case 32:
        	case 64:
        	case 128:
        	case 256:
        	case 512:
        		break;
        	default:
        		sizeInMB = DEFAULT_SIZE_IN_MB;
        }
	return sizeInMB * 64;
}



static int  CheckInit(void)
{
	static int initialised = 0;
	
	int i;
	int fail = 0;
	int nBlocks; 
	int nAllocated = 0;
	
	if(initialised) 
	{
		return YAFFS_OK;
	}
	
	
	nBlocks = nandemul_CalcNBlocks();
	
	ned.block = YMALLOC(sizeof(nandemul_Block *) * nBlocks);
	
	if(!ned.block) return 0;
	
	for(i=0; i <nBlocks; i++)
	{
		ned.block[i] = NULL;
	}
	
	for(i=0; i <nBlocks && !fail; i++)
	{
		if((ned.block[i] = YMALLOC(sizeof(nandemul_Block))) == 0)
		{
			fail = 1;
		}
		else
		{
			nandemul_ReallyEraseBlock(i);
			ned.block[i]->damaged = 0;
			nAllocated++;
		}
	}
	
	if(fail)
	{
		for(i = 0; i < nAllocated; i++)
		{
			YFREE(ned.block[i]);
		}
		YFREE(ned.block);
		
		T(YAFFS_TRACE_ALWAYS,("Allocation failed, could only allocate %dMB of %dMB requested.\n",
		   nAllocated/64,sizeInMB));
		return 0;
	}
	
	ned.nBlocks = nBlocks;
	
	initialised = 1;
	
	return 1;
}

int nandemul_WriteChunkToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_Spare *spare)
{
	int blk;
	int pg;
	int i;
	
	__u8 *x;
	
	__u8 *spareAsBytes = (__u8 *)spare;

	
	CheckInit();
	
	blk = chunkInNAND/32;
	pg = chunkInNAND%32;
	
	
	if(data)
	{
		x = ned.block[blk]->page[pg].data;
		
		for(i = 0; i < 512; i++)
		{
			x[i] &=data[i];
		}
		
		ned.block[blk]->page[pg].count[0]++;
		ned.block[blk]->page[pg].count[1]++;
		ned.block[blk]->page[pg].empty = 0;
	}
	
	
	if(spare)
	{
		x = &ned.block[blk]->page[pg].data[512];
			
		for(i = 0; i < 16; i++)
		{
			x[i] &=spareAsBytes[i];
		}
		ned.block[blk]->page[pg].count[2]++;
	}
	
	if(spare || data)
	{
		nandemul_yield(1);
	}

	return YAFFS_OK;
}


int nandemul_ReadChunkFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare)
{
	int blk;
	int pg;

	
	CheckInit();
	
	blk = chunkInNAND/32;
	pg = chunkInNAND%32;
	
	
	if(data)
	{
		memcpy(data,ned.block[blk]->page[pg].data,512);
	}
	
	
	if(spare)
	{
		memcpy(spare,&ned.block[blk]->page[pg].data[512],16);
	}

	return YAFFS_OK;
}


static int nandemul_CheckChunkErased(yaffs_Device *dev,int chunkInNAND)
{
	int blk;
	int pg;
	int i;

	
	CheckInit();
	
	blk = chunkInNAND/32;
	pg = chunkInNAND%32;
	
	
	for(i = 0; i < 528; i++)
	{
		if(ned.block[blk]->page[pg].data[i] != 0xFF)
		{
			return YAFFS_FAIL;
		}
	}

	return YAFFS_OK;

}

int nandemul_EraseBlockInNAND(yaffs_Device *dev, int blockNumber)
{
	
	CheckInit();
	
	if(blockNumber < 0 || blockNumber >= ned.nBlocks)
	{
		T(YAFFS_TRACE_ALWAYS,("Attempt to erase non-existant block %d\n",blockNumber));
	}
	else if(ned.block[blockNumber]->damaged)
	{
		T(YAFFS_TRACE_ALWAYS,("Attempt to erase damaged block %d\n",blockNumber));
	}
	else
	{
		nandemul_ReallyEraseBlock(blockNumber);
	}
	
	return YAFFS_OK;
}

int nandemul_InitialiseNAND(yaffs_Device *dev)
{
	return YAFFS_OK;
}

#endif //YAFFS_RAM_ENABLED

