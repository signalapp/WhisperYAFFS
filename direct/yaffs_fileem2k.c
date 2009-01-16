/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * This provides a YAFFS nand emulation on a file for emulating 2kB pages.
 * This is only intended as test code to test persistence etc.
 */

const char *yaffs_flashif2_c_version = "$Id: yaffs_fileem2k.c,v 1.18 2009-01-16 02:26:56 charles Exp $";


#include "yportenv.h"

#include "yaffs_flashif2.h"
#include "yaffs_guts.h"
#include "devextras.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#include "yaffs_fileem2k.h"
#include "yaffs_packedtags2.h"



#define REPORT_ERROR 0

typedef struct 
{
	__u8 data[PAGE_SIZE]; // Data + spare
} yflash_Page;

typedef struct
{
	yflash_Page page[PAGES_PER_BLOCK]; // The pages in the block
	
} yflash_Block;



#define MAX_HANDLES 20
#define BLOCKS_PER_HANDLE (32*8)

typedef struct
{
	int handle[MAX_HANDLES];
	int nBlocks;
} yflash_Device;

static yflash_Device filedisk;

int yaffs_testPartialWrite = 0;

extern int random_seed;
extern int simulate_power_failure;
static int initialised = 0;
static int remaining_ops;
static int nops_so_far;

int ops_multiplier;


static void yflash2_MaybePowerFail(void)
{

   nops_so_far++;
   
   
   remaining_ops--;
   if(simulate_power_failure &&
      remaining_ops < 1){
       printf("Simulated power failure after %d operations\n",nops_so_far);
    	exit(0);
  }
}





static __u8 localBuffer[PAGE_SIZE];

static char *NToName(char *buf,int n)
{
	sprintf(buf,"emfile-2k-%d",n);
	return buf;
}

static char dummyBuffer[BLOCK_SIZE];

static int GetBlockFileHandle(int n)
{
	int h;
	int requiredSize;
	
	char name[40];
	NToName(name,n);
	int fSize;
	int i;
	
	h =  open(name, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	if(h >= 0){
		fSize = lseek(h,0,SEEK_END);
		requiredSize = BLOCKS_PER_HANDLE * BLOCK_SIZE;
		if(fSize < requiredSize){
		   for(i = 0; i < BLOCKS_PER_HANDLE; i++)
		   	if(write(h,dummyBuffer,BLOCK_SIZE) != BLOCK_SIZE)
				return -1;
			
		}
	}
	
	return h;

}

static int  CheckInit(void)
{
	static int initialised = 0;
	int i;

	int blk;

	
	if(initialised) 
	{
		return YAFFS_OK;
	}

	initialised = 1;
	

	srand(random_seed);
	remaining_ops = (rand() % 1000) * 5;
  	memset(dummyBuffer,0xff,sizeof(dummyBuffer));

	
	
	filedisk.nBlocks = SIZE_IN_MB * BLOCKS_PER_MB;

	for(i = 0; i <  MAX_HANDLES; i++)
		filedisk.handle[i] = -1;
	
	for(i = 0,blk = 0; blk < filedisk.nBlocks; blk+=BLOCKS_PER_HANDLE,i++)
		filedisk.handle[i] = GetBlockFileHandle(i);
	
	
	return 1;
}


int yflash2_GetNumberOfBlocks(void)
{
	CheckInit();
	
	return filedisk.nBlocks;
}

int yflash2_WriteChunkWithTagsToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_ExtendedTags *tags)
{
	int written;
	int pos;
	int h;
	int i;
	int nRead;
	int error;
	
	T(YAFFS_TRACE_MTD,(TSTR("write chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));

	CheckInit();
	
	
	if(dev->inbandTags){
		
		yaffs_PackedTags2TagsPart * pt2tp;
		pt2tp = (yaffs_PackedTags2TagsPart *)&data[dev->nDataBytesPerChunk];
		yaffs_PackTags2TagsPart(pt2tp,tags);
		
		pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
		h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
				  			
		lseek(h,pos,SEEK_SET);
		written = write(h,data,dev->totalBytesPerChunk);

		
		if(yaffs_testPartialWrite){
			close(h);
			exit(1);
		}
		
		if(written != dev->totalBytesPerChunk) return YAFFS_FAIL;


	}
	
	else {
		/* First do a write of a partial page */
		int n_partials;
		int bpos;

		if(data)
		{
			pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
			h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			
			memcpy(localBuffer,data, dev->nDataBytesPerChunk);
			
			n_partials = rand()%20;
			
			for(i = 0; i < n_partials; i++){
				bpos = rand() % dev->nDataBytesPerChunk;
				
				localBuffer[bpos] |= (1 << (rand() & 7));
			}
		  
			if(REPORT_ERROR && memcmp(localBuffer,data,dev->nDataBytesPerChunk))
				printf("nand simulator: data does not match\n");
			
			lseek(h,pos,SEEK_SET);
			written = write(h,localBuffer,dev->nDataBytesPerChunk);
		
			if(yaffs_testPartialWrite){
				close(h);
				exit(1);
			}


			if(written != dev->nDataBytesPerChunk) return YAFFS_FAIL;
		}
		yflash2_MaybePowerFail();
	
		if(tags)
		{
			pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE + PAGE_DATA_SIZE ;
			h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			lseek(h,pos,SEEK_SET);

			if( 0 && dev->isYaffs2)
			{
			
				written = write(h,tags,sizeof(yaffs_ExtendedTags));
				if(written != sizeof(yaffs_ExtendedTags)) return YAFFS_FAIL;
			}
			else
			{
				yaffs_PackedTags2 pt;
				yaffs_PackTags2(&pt,tags);
				__u8 * ptab = (__u8 *)&pt;

				nRead = read(h,localBuffer,sizeof(pt));
				for(i = error = 0; REPORT_ERROR && i < sizeof(pt) && !error; i++){
					if(localBuffer[i] != 0xFF){
						printf("nand simulation: chunk %d oob byte %d was %0x2\n",
							chunkInNAND,i,localBuffer[i]);
							error = 1;
					}
				}
		
				for(i = 0; i < sizeof(pt); i++)
				localBuffer[i] &= ptab[i];
				
				n_partials = rand()% sizeof(pt);
			
				for(i = 0; i < n_partials; i++){
					bpos = rand() % sizeof(pt);
				
					localBuffer[bpos] |= (1 << (rand() & 7));
				}			
			 
				if(REPORT_ERROR && memcmp(localBuffer,&pt,sizeof(pt)))
					printf("nand sim: tags corruption\n");
				
				lseek(h,pos,SEEK_SET);
			
				written = write(h,localBuffer,sizeof(pt));
				if(written != sizeof(pt)) return YAFFS_FAIL;
			}
		}
		
		//yflash2_MaybePowerFail();
		
		/* Next do the whole write */
		if(data)
		{
			pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
			h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			
			memset(localBuffer,0xFF, PAGE_SIZE);		
			for(i = 0; i < dev->nDataBytesPerChunk; i++){
				localBuffer[i] &= data[i];
			}
		  
			if(REPORT_ERROR && memcmp(localBuffer,data,dev->nDataBytesPerChunk))
				printf("nand simulator: data does not match\n");
			
			lseek(h,pos,SEEK_SET);
			written = write(h,localBuffer,dev->nDataBytesPerChunk);
		
			if(yaffs_testPartialWrite){
				close(h);
				exit(1);
			}


			if(written != dev->nDataBytesPerChunk) return YAFFS_FAIL;
		}
	
		if(tags)
		{
			pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE + PAGE_DATA_SIZE ;
			h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			lseek(h,pos,SEEK_SET);

			if( 0 && dev->isYaffs2)
			{
			
				written = write(h,tags,sizeof(yaffs_ExtendedTags));
				if(written != sizeof(yaffs_ExtendedTags)) return YAFFS_FAIL;
			}
			else
			{
				yaffs_PackedTags2 pt;
				yaffs_PackTags2(&pt,tags);
				__u8 * ptab = (__u8 *)&pt;

				nRead = read(h,localBuffer,sizeof(pt));
				for(i = error = 0; REPORT_ERROR && i < sizeof(pt) && !error; i++){
					if(localBuffer[i] != 0xFF){
						printf("nand simulation: chunk %d oob byte %d was %0x2\n",
							chunkInNAND,i,localBuffer[i]);
							error = 1;
					}
				}
		
				for(i = 0; i < sizeof(pt); i++)
				localBuffer[i] &= ptab[i];
			 
				if(REPORT_ERROR && memcmp(localBuffer,&pt,sizeof(pt)))
					printf("nand sim: tags corruption\n");
				
				lseek(h,pos,SEEK_SET);
			
				written = write(h,localBuffer,sizeof(pt));
				if(written != sizeof(pt)) return YAFFS_FAIL;
			}
		}
		
		yflash2_MaybePowerFail();
	
	}
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


static int fail300 = 1;
static int fail320 = 1;

static int failRead10 = 2;

int yflash2_ReadChunkWithTagsFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_ExtendedTags *tags)
{
	int nread;
	int pos;
	int h;
	int localData = 0;
	int retval = YAFFS_OK;
	int nRead;
	
	T(YAFFS_TRACE_MTD,(TSTR("read chunk %d data %x tags %x" TENDSTR),chunkInNAND,(unsigned)data, (unsigned)tags));
	
	CheckInit();
	
	
	
	
	if(dev->inbandTags){
		/* Got to suck the tags out of the data area */
		if(!data) {
			localData=1;
			data = yaffs_GetTempBuffer(dev,__LINE__);
		}

		
		yaffs_PackedTags2TagsPart * pt2tp;
		pt2tp = (yaffs_PackedTags2TagsPart *)&data[dev->nDataBytesPerChunk];

		
		pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
		h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
		lseek(h,pos,SEEK_SET);

		nRead = read(h, data,dev->totalBytesPerChunk);

		yaffs_UnpackTags2TagsPart(tags,pt2tp);
		
		if(nread != dev->totalBytesPerChunk)
			retval = YAFFS_FAIL;
			
		if(localData)
			yaffs_ReleaseTempBuffer(dev,data,__LINE__);



	}
	
	else {
	
		if(data)
		{

			pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
			h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];		
			lseek(h,pos,SEEK_SET);
			nread = read(h,data,dev->nDataBytesPerChunk);
		
		
			if(nread != dev->nDataBytesPerChunk) 
				retval = YAFFS_FAIL;
		}
	
		if(tags)
		{
			pos = (chunkInNAND % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE + PAGE_DATA_SIZE;
			h = filedisk.handle[(chunkInNAND / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];		
			lseek(h,pos,SEEK_SET);

			if(0 && dev->isYaffs2)
			{
				nread= read(h,tags,sizeof(yaffs_ExtendedTags));
				if(nread != sizeof(yaffs_ExtendedTags))
					 retval =  YAFFS_FAIL;
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
				nread= read(h,&pt,sizeof(pt));
				yaffs_UnpackTags2(tags,&pt);
#ifdef SIMULATE_FAILURES
				if((chunkInNAND >> 6) == 100) {
					if(fail300 && tags->eccResult == YAFFS_ECC_RESULT_NO_ERROR){
						tags->eccResult = YAFFS_ECC_RESULT_FIXED;
						fail300 = 0;
					}
				}
				
				if((chunkInNAND >> 6) == 110) {
					if(fail320 && tags->eccResult == YAFFS_ECC_RESULT_NO_ERROR){
						tags->eccResult = YAFFS_ECC_RESULT_FIXED;
						fail320 = 0;
					}
				}
#endif
				if(failRead10>0 && chunkInNAND == 10){
					failRead10--;
					nread = 0;
				}
			
				if(nread != sizeof(pt))
					retval = YAFFS_FAIL;
			}
		}
	}
	


	return retval;	

}


int yflash2_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockNo)
{
	int written;
	int h;
	
	yaffs_PackedTags2 pt;

	CheckInit();
	
	memset(&pt,0,sizeof(pt));
	h = filedisk.handle[(blockNo / ( BLOCKS_PER_HANDLE))];
	lseek(h,((blockNo % BLOCKS_PER_HANDLE) * dev->nChunksPerBlock) * PAGE_SIZE + PAGE_DATA_SIZE,SEEK_SET);
	written = write(h,&pt,sizeof(pt));
		
	if(written != sizeof(pt)) return YAFFS_FAIL;
	
	
	return YAFFS_OK;
	
}

int yflash2_EraseBlockInNAND(yaffs_Device *dev, int blockNumber)
{

	int i;
	int h;
		
	CheckInit();
	
	printf("erase block %d\n",blockNumber);
	
	if(blockNumber == 320)
		fail320 = 1;
	
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
		

		h = filedisk.handle[(blockNumber / ( BLOCKS_PER_HANDLE))];
		lseek(h,((blockNumber % BLOCKS_PER_HANDLE) * dev->nChunksPerBlock) * PAGE_SIZE,SEEK_SET);		
		for(i = 0; i < dev->nChunksPerBlock; i++)
		{
			write(h,pg,PAGE_SIZE);
		}
		pos = lseek(h, 0,SEEK_CUR);
		
		return YAFFS_OK;
	}
	
}

int yflash2_InitialiseNAND(yaffs_Device *dev)
{
	CheckInit();
	
	return YAFFS_OK;
}




int yflash2_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockNo, yaffs_BlockState *state, __u32 *sequenceNumber)
{
	yaffs_ExtendedTags tags;
	int chunkNo;

	*sequenceNumber = 0;
	
	chunkNo = blockNo * dev->nChunksPerBlock;
	
	yflash2_ReadChunkWithTagsFromNAND(dev,chunkNo,NULL,&tags);
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

