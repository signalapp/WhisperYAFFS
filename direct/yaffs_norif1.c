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
 * This is an interface module for handling NOR in yaffs1 mode.
 */

/* First set up for M18 with 1k chunks and 16-byte spares.
 *
 * NB We're using the oddball M18 modes of operation here 
 * The chip is 64MB based at 0x0000, but YAFFS only going to use the top half
 * ie. YAFFS will be from 32MB to 64MB.
 *
 * The M18 has two ways of writing data. Every Programming Region (1kbytes) 
 * can be programmed in two modes:
 * * Object Mode 1024 bytes of write once data.
 * * Control Mode: 512bytes of bit-writeable data. 
 *    This is arranged as 32 * (16 bytes of bit-writable followed by 16 bytes of "dont touch")
 * 
 * The block size is 256kB, making 128 blocks in the 32MB YAFFS area.
 * Each block comprises:
 *   Offset   0k: 248 x 1k data pages
 *   Offset 248k: 248 x 32-byte spare areas implemented as 16 bytes of spare followed by 16 bytes untouched)
 *   Offset 248k + (248 * 32): Format marker
 *   
 */

const char *yaffs_norif1_c_version = "$Id: yaffs_norif1.c,v 1.4 2009-01-09 02:54:14 charles Exp $";

#include "yaffs_norif1.h"

#include "yportenv.h"

#include "yaffs_flashif.h"
#include "yaffs_guts.h"

#include "devextras.h"

#define SPARE_BYTES_PER_CHUNK	16
#define M18_SKIP		16
#define PROG_REGION_SIZE	1024
#define BLOCK_SIZE_IN_BYTES 	(256*1024)
#define CHUNKS_PER_BLOCK	248
#define SPARE_AREA_OFFSET	(CHUNKS_PER_BLOCK * PROG_REGION_SIZE)

#define FORMAT_OFFSET		(SPARE_AREA_OFFSET + CHUNKS_PER_BLOCK * (SPARE_BYTES_PER_CHUNK + M18_SKIP))

#define FORMAT_VALUE		0x1234

#define DATA_BYTES_PER_CHUNK	1024
#define BLOCKS_IN_DEVICE        (8*1024/256)


#define YNOR_PREMARKER          (0xF6)
#define YNOR_POSTMARKER         (0xF0)


#if 1

/* Compile this for a simulation */
#include "ynorsim.h"
#define ynorif1_FlashInit() ynorsim_Initialise()
#define ynorif1_FlashDeinit() ynorsim_Shutdown()
#define ynorif1_FlashWrite32(addr,buf,nwords) ynorsim_Write32(addr,buf,nwords) 
#define ynorif1_FlashRead32(addr,buf,nwords) ynorsim_Read32(addr,buf,nwords) 
#define ynorif1_FlashEraseBlock(addr) ynorsim_EraseBlock(addr)
#define DEVICE_BASE     ynorsim_GetBase()
#else

/* Compile this for running on blob, hacked for yaffs access */
#include "../blob/yflashrw.h"
#define ynorif1_FlashInit()  do{} while(0)
#define ynorif1_FlashDeinit() do {} while(0)
#define ynorif1_FlashWrite32(addr,buf,nwords) Y_FlashWrite(addr,buf,nwords) 
#define ynorif1_FlashRead32(addr,buf,nwords)  Y_FlashRead(addr,buf,nwords) 
#define ynorif1_FlashEraseBlock(addr)         Y_FlashErase(addr,BLOCK_SIZE_IN_BYTES)
#define DEVICE_BASE     (32 * 1024 * 1024)
#endif

__u32 *Block2Addr(yaffs_Device *dev, int blockNumber)
{
	__u32 addr;
	
	addr = (__u32) DEVICE_BASE;
	addr += blockNumber * BLOCK_SIZE_IN_BYTES;
	
	return (__u32 *) addr;
}

__u32 *Block2FormatAddr(yaffs_Device *dev,int blockNumber)
{
	__u32 addr;
	
	addr = (__u32) Block2Addr(dev,blockNumber);
	addr += FORMAT_OFFSET;
	
	return (__u32 *)addr;
}
__u32 *Chunk2DataAddr(yaffs_Device *dev,int chunkId)
{
	unsigned block;
	unsigned chunkInBlock;
	__u32  addr;
	
	block = chunkId/dev->nChunksPerBlock;
	chunkInBlock = chunkId % dev->nChunksPerBlock;
	
	addr = (__u32) Block2Addr(dev,block);
	addr += chunkInBlock * DATA_BYTES_PER_CHUNK;
	
	return (__u32 *)addr;
}

__u32 *Chunk2SpareAddr(yaffs_Device *dev,int chunkId)
{
	unsigned block;
	unsigned chunkInBlock;
	__u32 addr;
	
	block = chunkId/dev->nChunksPerBlock;
	chunkInBlock = chunkId % dev->nChunksPerBlock;
	
	addr = (__u32) Block2Addr(dev,block);
	addr += SPARE_AREA_OFFSET;
	addr += chunkInBlock * (SPARE_BYTES_PER_CHUNK + M18_SKIP);
	return (__u32 *)addr;
}


void ynorif1_AndBytes(__u8*target, const __u8   *src, int nbytes)
{
        while(nbytes > 0){
                *target &= *src;
                target++;
                src++;
                nbytes--;
        }
}

int ynorif1_WriteChunkToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, const yaffs_Spare *spare)
{
        __u32 *dataAddr = Chunk2DataAddr(dev,chunkInNAND);
        __u32 *spareAddr = Chunk2SpareAddr(dev,chunkInNAND);
        
        yaffs_Spare tmpSpare;
        
        /* We should only be getting called for one of 3 reasons:
         * Writing a chunk: data and spare will not be NULL
         * Writing a deletion marker: data will be NULL, spare not NULL
         * Writing a bad block marker: data will be NULL, spare not NULL
         */
         
        if(sizeof(yaffs_Spare) != 16)
                YBUG();
        
        if(data && spare)
        {
                if(spare->pageStatus != 0xff)
                        YBUG();
                /* Write a pre-marker */
                memset(&tmpSpare,0xff,sizeof(tmpSpare));
                tmpSpare.pageStatus = YNOR_PREMARKER;
                ynorif1_FlashWrite32(spareAddr,(__u32 *)&tmpSpare,sizeof(yaffs_Spare)/4);

                /* Write the data */            
                ynorif1_FlashWrite32(dataAddr,(__u32 *)data,dev->totalBytesPerChunk / 4);
                
                
                memcpy(&tmpSpare,spare,sizeof(yaffs_Spare));
                
                /* Write the real tags, but override the premarker*/
                tmpSpare.pageStatus = YNOR_PREMARKER;
                ynorif1_FlashWrite32(spareAddr,(__u32 *)&tmpSpare,sizeof(yaffs_Spare)/4);
                
                /* Write a post-marker */
                tmpSpare.pageStatus = YNOR_POSTMARKER;
                ynorif1_FlashWrite32(spareAddr,(__u32 *)&tmpSpare,sizeof(tmpSpare)/4);  

        } else if(spare){
                /* This has to be a read-modify-write operation to handle NOR-ness */

                ynorif1_FlashRead32(spareAddr,(__u32 *)&tmpSpare,16/ 4);
                
                ynorif1_AndBytes((__u8 *)&tmpSpare,(__u8 *)spare,sizeof(yaffs_Spare));
                
                ynorif1_FlashWrite32(spareAddr,(__u32 *)&tmpSpare,16/ 4);
        }
        else {
                YBUG();
        }
        

	return YAFFS_OK;	

}

int ynorif1_ReadChunkFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare)
{

	__u32 *dataAddr = Chunk2DataAddr(dev,chunkInNAND);
	__u32 *spareAddr = Chunk2SpareAddr(dev,chunkInNAND);
	
	if(data)
	{
		ynorif1_FlashRead32(dataAddr,(__u32 *)data,dev->totalBytesPerChunk / 4);
	}
	
        if(spare)
        {
                ynorif1_FlashRead32(spareAddr,(__u32 *)spare,16/ 4);
                
                /* If the page status is YNOR_POSTMARKER then it was written properly
                 * so change that to 0xFF so that the rest of yaffs is happy.
                 */
                if(spare->pageStatus == YNOR_POSTMARKER)
                        spare->pageStatus = 0xFF;
		else if(spare->pageStatus != 0xff &&
			(spare->pageStatus | YNOR_PREMARKER) != 0xff)
			spare->pageStatus = YNOR_PREMARKER;
        }
        

	return YAFFS_OK;	

}

static int ynorif1_FormatBlock(yaffs_Device *dev, int blockNumber)
{
	__u32 *blockAddr = Block2Addr(dev,blockNumber);
	__u32 *formatAddr = Block2FormatAddr(dev,blockNumber);
	__u32 formatValue = FORMAT_VALUE;
	
	ynorif1_FlashEraseBlock(blockAddr);
	ynorif1_FlashWrite32(formatAddr,&formatValue,1);
	
	return YAFFS_OK;
}

static int ynorif1_UnformatBlock(yaffs_Device *dev, int blockNumber)
{
	__u32 *formatAddr = Block2FormatAddr(dev,blockNumber);
	__u32 formatValue = 0;
	
	ynorif1_FlashWrite32(formatAddr,&formatValue,1);
	
	return YAFFS_OK;
}

static int ynorif1_IsBlockFormatted(yaffs_Device *dev, int blockNumber)
{
	__u32 *formatAddr = Block2FormatAddr(dev,blockNumber);
	__u32 formatValue;
	
	
	ynorif1_FlashRead32(formatAddr,&formatValue,1);
	
	return (formatValue == FORMAT_VALUE);
}

int ynorif1_EraseBlockInNAND(yaffs_Device *dev, int blockNumber)
{

	if(blockNumber < 0 || blockNumber >= BLOCKS_IN_DEVICE)
	{
		T(YAFFS_TRACE_ALWAYS,("Attempt to erase non-existant block %d\n",blockNumber));
		return YAFFS_FAIL;
	}
	else
	{
		ynorif1_UnformatBlock(dev,blockNumber);
		ynorif1_FormatBlock(dev,blockNumber);
		return YAFFS_OK;
	}
	
}

int ynorif1_InitialiseNAND(yaffs_Device *dev)
{
	int i;
	
	ynorif1_FlashInit();
	/* Go through the blocks formatting them if they are not formatted */
	for(i = dev->startBlock; i <= dev->endBlock; i++){
		if(!ynorif1_IsBlockFormatted(dev,i)){
			ynorif1_FormatBlock(dev,i);
		}
	}
	return YAFFS_OK;
}

int ynorif1_DeinitialiseNAND(yaffs_Device *dev)
{
	
	ynorif1_FlashDeinit();

	return YAFFS_OK;
}


