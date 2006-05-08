/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
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

const char *yaffs_checkptrw_c_version =
    "$Id: yaffs_checkptrw.c,v 1.1 2006-05-08 10:13:34 charles Exp $";


#include "yaffs_checkptrw.h"


int yaffs_CheckpointSpaceOk(yaffs_Device *dev)
{

	T(YAFFS_TRACE_CHECKPOINT,(TSTR("checkpt blocks %d %d %d %d" TENDSTR),
		dev->checkpointStartBlock, dev->checkpointEndBlock,
		dev->startBlock,dev->endBlock));
		
	if(dev->checkpointStartBlock >= dev->checkpointEndBlock)
		return 0;
		
	if(dev->checkpointStartBlock >= dev->startBlock && 
	   dev->checkpointStartBlock <= dev->endBlock)
	   	return 0;

	if(dev->checkpointEndBlock >= dev->startBlock && 
	   dev->checkpointEndBlock <= dev->endBlock)
	   	return 0;
	   	
	return 1;
}

int yaffs_CheckpointOpen(yaffs_Device *dev, int forWriting)
{
	int i;
	
	/* Got the functions we need? */
	if (!dev->writeChunkWithTagsToNAND ||
	    !dev->readChunkWithTagsFromNAND ||
	    !dev->eraseBlockInNAND)
		return 0;
	
	/* Got a valid checkpoint data region? */
	if(!yaffs_CheckpointSpaceOk(dev))
		return 0;
			
	if(!dev->checkpointBuffer)
		dev->checkpointBuffer = YMALLOC(dev->nBytesPerChunk);
	if(!dev->checkpointBuffer)
		return 0;
	
	dev->checkpointPage = dev->checkpointStartBlock * dev->nChunksPerBlock;
	
	dev->checkpointOpenForWrite = forWriting;
	
	dev->checkpointByteCount = 0;
	
	/* Erase all the blocks in the checkpoint area */
	if(forWriting){
		T(YAFFS_TRACE_CHECKPOINT,(TSTR("erasing checkpt data"TENDSTR)));
		for( i = dev->checkpointStartBlock; i <= dev->checkpointEndBlock; i++)
			dev->eraseBlockInNAND(dev,i);
		
		dev->checkpointByteOffset = 0;
		
		memset(dev->checkpointBuffer,0,dev->nBytesPerChunk);
		
	} else {
		/* Set to a value that will kick off a read */
		dev->checkpointByteOffset = dev->nBytesPerChunk;
	}
	
	return 1;
}

static int yaffs_CheckpointFlushBuffer(yaffs_Device *dev)
{

	int blockNo;
	int goodBlockFound;
	yaffs_BlockState state;
	__u32 seqenceNumber;

	yaffs_ExtendedTags tags;
	tags.chunkDeleted = 0;
	tags.objectId = YAFFS_OBJECTID_CHECKPOINT_DATA;
	tags.chunkId = dev->checkpointPage + 1;
	tags.sequenceNumber = 1;
	tags.byteCount = dev->nBytesPerChunk;
	
	/* printf("write checkpoint page %d\n",dev->checkpointPage); */
	
	if(dev->checkpointPage%dev->nChunksPerBlock == 0){
		/* Start of a new block, do a block validity check */
		blockNo = dev->checkpointPage/dev->nChunksPerBlock;
		goodBlockFound = 0;
		while(blockNo <= dev->checkpointEndBlock && !goodBlockFound){
			dev->queryNANDBlock(dev,blockNo,&state,&tags);
			if(state != YAFFS_BLOCK_STATE_DEAD)
				goodBlockFound = 1;
			else {
				blockNo++;
				dev->checkpointPage += dev->nChunksPerBlock;
			}
		}
		
		if(!goodBlockFound)
			return 0;
	}
	
	dev->writeChunkWithTagsToNAND(dev, dev->checkpointPage, 
				      dev->checkpointBuffer,&tags);
	dev->checkpointByteOffset = 0;
	dev->checkpointPage++;	   
	memset(dev->checkpointBuffer,0,dev->nBytesPerChunk);
	
	return 1;
}

int yaffs_CheckpointWrite(yaffs_Device *dev,const void *data, int nBytes)
{
	int i=0;
	int ok = 1;

	
	__u8 * dataBytes = (__u8 *)data;
	
	

	if(!dev->checkpointBuffer)
		return 0;

	while(i < nBytes && ok) {
		

		
		 dev->checkpointBuffer[dev->checkpointByteOffset] = *dataBytes ;
		dev->checkpointByteOffset++;
		i++;
		dataBytes++;
		dev->checkpointByteCount++;
		
		
		if(dev->checkpointByteOffset < 0 ||
		   dev->checkpointByteOffset >= dev->nBytesPerChunk) 
			ok = yaffs_CheckpointFlushBuffer(dev);

	}
	
	return 	i;
}

int yaffs_CheckpointRead(yaffs_Device *dev, void *data, int nBytes)
{
	int i=0;
	int ok = 1;
	yaffs_ExtendedTags tags;
	yaffs_BlockState state;
	int blockNo;
	int goodBlockFound;

	__u8 *dataBytes = (__u8 *)data;
		
	if(!dev->checkpointBuffer)
		return 0;

	while(i < nBytes && ok) {
		if(dev->checkpointByteOffset < 0 ||
		   dev->checkpointByteOffset >= dev->nBytesPerChunk) {
			if(dev->checkpointPage%dev->nChunksPerBlock == 0){
				/* Start of a new block, do a block validity check */
				blockNo = dev->checkpointPage/dev->nChunksPerBlock;
				goodBlockFound = 0;
				while(blockNo <= dev->checkpointEndBlock && !goodBlockFound){
					dev->queryNANDBlock(dev,blockNo,&state,&tags);
					if(state != YAFFS_BLOCK_STATE_DEAD)
						goodBlockFound = 1;
					else {
						blockNo++;
						dev->checkpointPage += dev->nChunksPerBlock;
					}
				}
		
				if(!goodBlockFound)
					return 0;
			}


	   		/* read in the next chunk */
	   		/* printf("read checkpoint page %d\n",dev->checkpointPage); */
			dev->readChunkWithTagsFromNAND(dev, dev->checkpointPage, 
						       dev->checkpointBuffer,
						      &tags);
						      
			if(tags.objectId != YAFFS_OBJECTID_CHECKPOINT_DATA ||
			   tags.chunkId != (dev->checkpointPage + 1))
			   ok = 0;

			dev->checkpointByteOffset = 0;
			dev->checkpointPage++;
		}
		
		if(ok){
			*dataBytes = dev->checkpointBuffer[dev->checkpointByteOffset];
			dev->checkpointByteOffset++;
			i++;
			dataBytes++;
			dev->checkpointByteCount++;
		}
	}
	
	return 	i;
}

int yaffs_CheckpointClose(yaffs_Device *dev)
{

	if(dev->checkpointOpenForWrite){	
		if(dev->checkpointByteOffset != 0)
			yaffs_CheckpointFlushBuffer(dev);
	}

	T(YAFFS_TRACE_CHECKPOINT,(TSTR("checkpoint byte count %d" TENDSTR),
			dev->checkpointByteCount));
			
	if(dev->checkpointBuffer){
		/* flush the buffer */	
		YFREE(dev->checkpointBuffer);
		dev->checkpointBuffer = NULL;
		return 1;
	}
	else
		return 0;
	
}

int yaffs_CheckpointInvalidateStream(yaffs_Device *dev)
{
	/* Erase the first checksum block */

	T(YAFFS_TRACE_CHECKPOINT,(TSTR("checkpoint invalidate"TENDSTR)));

	if(!yaffs_CheckpointSpaceOk(dev))
		return 0;

	if(dev->eraseBlockInNAND){
		T(YAFFS_TRACE_CHECKPOINT,(TSTR("erasing checkpt data"TENDSTR)));

		return dev->eraseBlockInNAND(dev, dev->checkpointStartBlock);
	}
	else
		return 0;
}



