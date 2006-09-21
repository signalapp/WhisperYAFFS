/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 * yaffs_ramdisk.c: yaffs ram disk component
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

// This provides a YAFFS nand emulation on a file for emulating 2kB pages.
// THis is only intended as test code to test persistence etc.

const char *yaffs_flashif_c_version = "$Id: yaffs_fileem2k.c,v 1.5 2006-09-21 08:13:59 charles Exp $";


#include "yportenv.h"

#include "yaffs_flashif.h"
#include "yaffs_guts.h"
#include "devextras.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#include "yaffs_fileem2k.h"
#include "yaffs_packedtags2.h"



typedef struct 
{
	__u8 data[PAGE_SIZE]; // Data + spare
} yflash_Page;

typedef struct
{
	yflash_Page page[PAGES_PER_BLOCK]; // The pages in the block
	
} yflash_Block;



typedef struct
{
	int handle;
	int nBlocks;
} yflash_Device;

static yflash_Device filedisk;

int yaffs_testPartialWrite = 0;

static int  CheckInit(void)
{
	static int initialised = 0;
	
	int i;

	
	int fSize;
	int written;
	
	yflash_Page p;
	
	if(initialised) 
	{
		return YAFFS_OK;
	}

	initialised = 1;
	
	
	filedisk.nBlocks = SIZE_IN_MB * BLOCKS_PER_MB;
	
	filedisk.handle = open("yaffsemfile2k", O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	
	if(filedisk.handle < 0)
	{
		perror("Failed to open yaffs emulation file");
		return YAFFS_FAIL;
	}
	
	
	fSize = lseek(filedisk.handle,0,SEEK_END);
	
	if(fSize < filedisk.nBlocks * BLOCK_SIZE)
	{
		printf("Creating yaffs emulation file\n");
		
		lseek(filedisk.handle,0,SEEK_SET);
		
		memset(&p,0xff,sizeof(yflash_Page));
		
		for(i = 0; i <  filedisk.nBlocks * BLOCK_SIZE; i+= PAGE_SIZE)
		{
			written = write(filedisk.handle,&p,sizeof(yflash_Page));
			
			if(written != sizeof(yflash_Page))
			{
				printf("Write failed\n");
				return YAFFS_FAIL;
			}
		}		
	}
	else
	{
		filedisk.nBlocks = fSize/(BLOCK_SIZE);
	}
	
	return 1;
}


int yflash_GetNumberOfBlocks(void)
{
	CheckInit();
	
	return filedisk.nBlocks;
}

int yflash_WriteChunkWithTagsToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, yaffs_ExtendedTags *tags)
{
	int written;
	int pos;

	CheckInit();
	
	
	
	if(data)
	{
		pos = chunkInNAND * PAGE_SIZE;
		lseek(filedisk.handle,pos,SEEK_SET);
		written = write(filedisk.handle,data,dev->nBytesPerChunk);
		
		if(yaffs_testPartialWrite){
			close(filedisk.handle);
			exit(1);
		}
		
		if(written != dev->nBytesPerChunk) return YAFFS_FAIL;
	}
	
	if(tags)
	{
		pos = chunkInNAND * PAGE_SIZE + PAGE_DATA_SIZE;
		lseek(filedisk.handle,pos,SEEK_SET);
		if( 0 && dev->isYaffs2)
		{
			
			written = write(filedisk.handle,tags,sizeof(yaffs_ExtendedTags));
			if(written != sizeof(yaffs_ExtendedTags)) return YAFFS_FAIL;
		}
		else
		{
			yaffs_PackedTags2 pt;
			yaffs_PackTags2(&pt,tags);

			written = write(filedisk.handle,&pt,sizeof(pt));
			if(written != sizeof(pt)) return YAFFS_FAIL;
		}
	}
	

	return YAFFS_OK;	

}

int yflash_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockNo)
{
	int written;
	
	yaffs_PackedTags2 pt;

	CheckInit();
	
	memset(&pt,0,sizeof(pt));
	lseek(filedisk.handle,(blockNo * dev->nChunksPerBlock) * PAGE_SIZE + PAGE_DATA_SIZE,SEEK_SET);
	written = write(filedisk.handle,&pt,sizeof(pt));
		
	if(written != sizeof(pt)) return YAFFS_FAIL;
	
	
	return YAFFS_OK;
	
}

int yaffs_CheckAllFF(const __u8 *ptr, int n)
{
	while(n)
	{
		n--;
		if(*ptr!=0xFF) return 0;
		ptr++;
	}
	return 1;
}


int yflash_ReadChunkWithTagsFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_ExtendedTags *tags)
{
	int nread;
	int pos;

	CheckInit();
	
	
	
	if(data)
	{
		pos = chunkInNAND * PAGE_SIZE;
		lseek(filedisk.handle,pos,SEEK_SET);
		nread = read(filedisk.handle,data,dev->nBytesPerChunk);
		
		if(nread != dev->nBytesPerChunk) return YAFFS_FAIL;
	}
	
	if(tags)
	{
		pos = chunkInNAND * PAGE_SIZE + PAGE_DATA_SIZE;
		lseek(filedisk.handle,pos,SEEK_SET);
		if(0 && dev->isYaffs2)
		{
			nread= read(filedisk.handle,tags,sizeof(yaffs_ExtendedTags));
			if(nread != sizeof(yaffs_ExtendedTags)) return YAFFS_FAIL;
			if(yaffs_CheckAllFF((__u8 *)tags,sizeof(yaffs_ExtendedTags)))
			{
				yaffs_InitialiseTags(tags);
			}
			else
			{
				tags->chunkUsed = 1;
			}
		}
		else
		{
			yaffs_PackedTags2 pt;
			nread= read(filedisk.handle,&pt,sizeof(pt));
			yaffs_UnpackTags2(tags,&pt);
			if(nread != sizeof(pt)) return YAFFS_FAIL;
		}
	}
	

	return YAFFS_OK;	

}


int yflash_EraseBlockInNAND(yaffs_Device *dev, int blockNumber)
{

	int i;
		
	CheckInit();
	
	if(blockNumber < 0 || blockNumber >= filedisk.nBlocks)
	{
		T(YAFFS_TRACE_ALWAYS,("Attempt to erase non-existant block %d\n",blockNumber));
		return YAFFS_FAIL;
	}
	else
	{
	
		__u8 pg[PAGE_SIZE];
		int syz = PAGE_SIZE;
		int pos;
		
		memset(pg,0xff,syz);
		
		pos = lseek(filedisk.handle, blockNumber * dev->nChunksPerBlock * PAGE_SIZE, SEEK_SET);
		
		for(i = 0; i < dev->nChunksPerBlock; i++)
		{
			write(filedisk.handle,pg,PAGE_SIZE);
		}
		pos = lseek(filedisk.handle, 0,SEEK_CUR);
		
		return YAFFS_OK;
	}
	
}

int yflash_InitialiseNAND(yaffs_Device *dev)
{
	CheckInit();
	
	return YAFFS_OK;
}




int yflash_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockNo, yaffs_BlockState *state, int *sequenceNumber)
{
	yaffs_ExtendedTags tags;
	int chunkNo;

	*sequenceNumber = 0;
	
	chunkNo = blockNo * dev->nChunksPerBlock;
	
	yflash_ReadChunkWithTagsFromNAND(dev,chunkNo,NULL,&tags);
	if(tags.blockBad)
	{
		*state = YAFFS_BLOCK_STATE_DEAD;
	}
	else if(!tags.chunkUsed)
	{
		*state = YAFFS_BLOCK_STATE_EMPTY;
	}
	else if(tags.chunkUsed)
	{
		*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
		*sequenceNumber = tags.sequenceNumber;
	}
	return YAFFS_OK;
}

