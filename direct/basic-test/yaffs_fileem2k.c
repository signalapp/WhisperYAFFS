/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
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

const char *yaffs_flashif2_c_version = "$Id: yaffs_fileem2k.c,v 1.24 2010-02-18 01:18:04 charles Exp $";


#include "yportenv.h"
#include "yaffs_trace.h"

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
	u8 data[PAGE_SIZE]; // Data + spare
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

int yaffs_test_partial_write = 0;

extern int random_seed;
extern int simulate_power_failure;
static int initialised = 0;
static int remaining_ops;
static int nops_so_far;

int ops_multiplier;


static void yflash2_MaybePowerFail(unsigned int nand_chunk, int failPoint)
{

   nops_so_far++;
   
   
   remaining_ops--;
   if(simulate_power_failure &&
      remaining_ops < 1){
       printf("Simulated power failure after %d operations\n",nops_so_far);
       printf("  power failed on nand_chunk %d, at fail point %d\n",
       			nand_chunk, failPoint);
    	exit(0);
  }
}





static u8 localBuffer[PAGE_SIZE];

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

int yflash2_WriteChunkWithTagsToNAND(yaffs_dev_t *dev,int nand_chunk,const u8 *data, const yaffs_ext_tags *tags)
{
	int written;
	int pos;
	int h;
	int i;
	int nRead;
	int error;
	
	T(YAFFS_TRACE_MTD,(TSTR("write chunk %d data %x tags %x" TENDSTR),nand_chunk,(unsigned)data, (unsigned)tags));

	CheckInit();
	
	
	if(dev->param.inband_tags){
		
		yaffs_packed_tags2_tags_only * pt2tp;
		pt2tp = (yaffs_packed_tags2_tags_only *)&data[dev->data_bytes_per_chunk];
		yaffs_pack_tags2_tags_only(pt2tp,tags);
		
		pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
		h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
				  			
		lseek(h,pos,SEEK_SET);
		written = write(h,data,dev->param.total_bytes_per_chunk);

		
		if(yaffs_test_partial_write){
			close(h);
			exit(1);
		}
		
		if(written != dev->param.total_bytes_per_chunk) return YAFFS_FAIL;


	}
	
	else {
		/* First do a write of a partial page */
		int n_partials;
		int bpos;

		if(data)
		{
			pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
			h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			
			memcpy(localBuffer,data, dev->data_bytes_per_chunk);
			
			n_partials = rand()%20;
			
			for(i = 0; i < n_partials; i++){
				bpos = rand() % dev->data_bytes_per_chunk;
				
				localBuffer[bpos] |= (1 << (rand() & 7));
			}
		  
			if(REPORT_ERROR && memcmp(localBuffer,data,dev->data_bytes_per_chunk))
				printf("nand simulator: data does not match\n");
			
			lseek(h,pos,SEEK_SET);
			written = write(h,localBuffer,dev->data_bytes_per_chunk);
		
			if(yaffs_test_partial_write){
				close(h);
				exit(1);
			}


			if(written != dev->data_bytes_per_chunk) return YAFFS_FAIL;
		}
		// yflash2_MaybePowerFail(nand_chunk,1);
	
		if(tags)
		{
			pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE + PAGE_DATA_SIZE ;
			h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			lseek(h,pos,SEEK_SET);

			if( 0 && dev->param.is_yaffs2)
			{
			
				written = write(h,tags,sizeof(yaffs_ext_tags));
				if(written != sizeof(yaffs_ext_tags)) return YAFFS_FAIL;
			}
			else
			{
				yaffs_packed_tags2 pt;
				yaffs_pack_tags2(&pt,tags, !dev->param.no_tags_ecc);
				u8 * ptab = (u8 *)&pt;

				nRead = read(h,localBuffer,sizeof(pt));
				for(i = error = 0; REPORT_ERROR && i < sizeof(pt) && !error; i++){
					if(localBuffer[i] != 0xFF){
						printf("nand simulation: chunk %d oob byte %d was %0x2\n",
							nand_chunk,i,localBuffer[i]);
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
		
		//yflash2_MaybePowerFail(nand_chunk,2);
		
		/* Next do the whole write */
		if(data)
		{
			pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
			h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			
			memset(localBuffer,0xFF, PAGE_SIZE);		
			for(i = 0; i < dev->data_bytes_per_chunk; i++){
				localBuffer[i] &= data[i];
			}
		  
			if(REPORT_ERROR && memcmp(localBuffer,data,dev->data_bytes_per_chunk))
				printf("nand simulator: data does not match\n");
			
			lseek(h,pos,SEEK_SET);
			written = write(h,localBuffer,dev->data_bytes_per_chunk);
		
			if(yaffs_test_partial_write){
				close(h);
				exit(1);
			}


			if(written != dev->data_bytes_per_chunk) return YAFFS_FAIL;
		}
	
		if(tags)
		{
			pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE + PAGE_DATA_SIZE ;
			h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
			lseek(h,pos,SEEK_SET);

			if( 0 && dev->param.is_yaffs2)
			{
			
				written = write(h,tags,sizeof(yaffs_ext_tags));
				if(written != sizeof(yaffs_ext_tags)) return YAFFS_FAIL;
			}
			else
			{
				yaffs_packed_tags2 pt;
				yaffs_pack_tags2(&pt,tags,!dev->param.no_tags_ecc);
				u8 * ptab = (u8 *)&pt;

				nRead = read(h,localBuffer,sizeof(pt));
				for(i = error = 0; REPORT_ERROR && i < sizeof(pt) && !error; i++){
					if(localBuffer[i] != 0xFF){
						printf("nand simulation: chunk %d oob byte %d was %0x2\n",
							nand_chunk,i,localBuffer[i]);
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
		
		yflash2_MaybePowerFail(nand_chunk,3);
	
	}
	return YAFFS_OK;	

}

int yaffs_check_all_ff(const u8 *ptr, int n)
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

int yflash2_ReadChunkWithTagsFromNAND(yaffs_dev_t *dev,int nand_chunk, u8 *data, yaffs_ext_tags *tags)
{
	int nread;
	int pos;
	int h;
	int localData = 0;
	int retval = YAFFS_OK;
	int nRead;
	
	T(YAFFS_TRACE_MTD,(TSTR("read chunk %d data %x tags %x" TENDSTR),nand_chunk,(unsigned)data, (unsigned)tags));
	
	CheckInit();
	
	
	
	
	if(dev->param.inband_tags){
		/* Got to suck the tags out of the data area */
		if(!data) {
			localData=1;
			data = yaffs_get_temp_buffer(dev,__LINE__);
		}

		
		yaffs_packed_tags2_tags_only * pt2tp;
		pt2tp = (yaffs_packed_tags2_tags_only *)&data[dev->data_bytes_per_chunk];

		
		pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
		h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];
		
		lseek(h,pos,SEEK_SET);

		nRead = read(h, data,dev->param.total_bytes_per_chunk);

		yaffs_unpack_tags2_tags_only(tags,pt2tp);
		
		if(nread != dev->param.total_bytes_per_chunk)
			retval = YAFFS_FAIL;
			
		if(localData)
			yaffs_release_temp_buffer(dev,data,__LINE__);



	}
	
	else {
	
		if(data)
		{

			pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE;
			h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];		
			lseek(h,pos,SEEK_SET);
			nread = read(h,data,dev->data_bytes_per_chunk);
		
		
			if(nread != dev->data_bytes_per_chunk) 
				retval = YAFFS_FAIL;
		}
	
		if(tags)
		{
			pos = (nand_chunk % (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE)) * PAGE_SIZE + PAGE_DATA_SIZE;
			h = filedisk.handle[(nand_chunk / (PAGES_PER_BLOCK * BLOCKS_PER_HANDLE))];		
			lseek(h,pos,SEEK_SET);

			if(0 && dev->param.is_yaffs2)
			{
				nread= read(h,tags,sizeof(yaffs_ext_tags));
				if(nread != sizeof(yaffs_ext_tags))
					 retval =  YAFFS_FAIL;
				if(yaffs_check_all_ff((u8 *)tags,sizeof(yaffs_ext_tags)))
				{
					yaffs_init_tags(tags);
				}
				else
				{
					tags->chunk_used = 1;
				}
			}
			else
			{
				yaffs_packed_tags2 pt;
				nread= read(h,&pt,sizeof(pt));
				yaffs_unpack_tags2(tags,&pt, !dev->param.no_tags_ecc);
#ifdef SIMULATE_FAILURES
				if((nand_chunk >> 6) == 100) {
					if(fail300 && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR){
						tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
						fail300 = 0;
					}
				}
				
				if((nand_chunk >> 6) == 110) {
					if(fail320 && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR){
						tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
						fail320 = 0;
					}
				}
#endif
				if(failRead10>0 && nand_chunk == 10){
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


int yflash2_MarkNANDBlockBad(struct yaffs_dev_s *dev, int block_no)
{
	int written;
	int h;
	
	yaffs_packed_tags2 pt;

	CheckInit();
	
	memset(&pt,0,sizeof(pt));
	h = filedisk.handle[(block_no / ( BLOCKS_PER_HANDLE))];
	lseek(h,((block_no % BLOCKS_PER_HANDLE) * dev->param.chunks_per_block) * PAGE_SIZE + PAGE_DATA_SIZE,SEEK_SET);
	written = write(h,&pt,sizeof(pt));
		
	if(written != sizeof(pt)) return YAFFS_FAIL;
	
	
	return YAFFS_OK;
	
}

int yflash2_EraseBlockInNAND(yaffs_dev_t *dev, int blockNumber)
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
	
		u8 pg[PAGE_SIZE];
		int syz = PAGE_SIZE;
		int pos;
		
		memset(pg,0xff,syz);
		

		h = filedisk.handle[(blockNumber / ( BLOCKS_PER_HANDLE))];
		lseek(h,((blockNumber % BLOCKS_PER_HANDLE) * dev->param.chunks_per_block) * PAGE_SIZE,SEEK_SET);		
		for(i = 0; i < dev->param.chunks_per_block; i++)
		{
			write(h,pg,PAGE_SIZE);
		}
		pos = lseek(h, 0,SEEK_CUR);
		
		return YAFFS_OK;
	}
	
}

int yflash2_InitialiseNAND(yaffs_dev_t *dev)
{
	CheckInit();
	
	return YAFFS_OK;
}




int yflash2_QueryNANDBlock(struct yaffs_dev_s *dev, int block_no, yaffs_block_state_t *state, u32 *seq_number)
{
	yaffs_ext_tags tags;
	int chunkNo;

	*seq_number = 0;
	
	chunkNo = block_no * dev->param.chunks_per_block;
	
	yflash2_ReadChunkWithTagsFromNAND(dev,chunkNo,NULL,&tags);
	if(tags.block_bad)
	{
		*state = YAFFS_BLOCK_STATE_DEAD;
	}
	else if(!tags.chunk_used)
	{
		*state = YAFFS_BLOCK_STATE_EMPTY;
	}
	else if(tags.chunk_used)
	{
		*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
		*seq_number = tags.seq_number;
	}
	return YAFFS_OK;
}

