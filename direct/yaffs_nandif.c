
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




#include "yportenv.h"
#include "yaffs_guts.h"
#include "devextras.h"


#include "yaffs_nandif.h"
#include "yaffs_packedtags2.h"


#if 0


static unsigned char *DevBufferIn(yaffs_Device *dev)
{
	yfsd_WinCEDevice *cedev = (yfsd_WinCEDevice *)(dev->genericDevice);
	return cedev->bufferIn;
}
static unsigned char *DevBufferOut(yaffs_Device *dev)
{
	yfsd_WinCEDevice *cedev = (yfsd_WinCEDevice *)(dev->genericDevice);
	return cedev->bufferOut;
}

static unsigned DevBufferSize(yaffs_Device *dev)
{
	yfsd_WinCEDevice *cedev = (yfsd_WinCEDevice *)(dev->genericDevice);
	return cedev->bufferSize;
}

#endif

/* NB For use with inband tags....
 * We assume that the data buffer is of size totalBytersPerChunk so that we can also
 * use it to load the tags.
 */
int ynandif_WriteChunkWithTagsToNAND(yaffs_Device * dev, int chunkInNAND,
				      const __u8 * data,
				      const yaffs_ExtendedTags * tags)
{

	int retval = 0;
	yaffs_PackedTags2 pt;
	void *spare;
	unsigned spareSize = 0;

	unsigned char *bufferIn   = DevBufferIn(dev);
	unsigned char *bufferOut  = DevBufferOut(dev);
	unsigned       bufferSize = DevBufferSize(dev);

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_WriteChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));
	    

	/* For yaffs2 writing there must be both data and tags.
	 * If we're using inband tags, then the tags are stuffed into
	 * the end of the data buffer.
	 */

	if(dev->inbandTags){
		yaffs_PackedTags2TagsPart *pt2tp;
		pt2tp = (yaffs_PackedTags2TagsPart *)(data + dev->nDataBytesPerChunk);
		yaffs_PackTags2TagsPart(pt2tp,tags);
		spare = NULL;
		spareSize = 0;
	}
	else{
		yaffs_PackTags2(&pt, tags);
		spare = &pt;
		spareSize = sizeof(yaffs_PackedTags2);
	}
	
	yramsim_WritePage(chunkInNAND,
					  data, dev->totalBytesPerChunk, spare, spareSize);

	return YAFFS_OK;
}

int ynandif_ReadChunkWithTagsFromNAND(yaffs_Device * dev, int chunkInNAND,
				       __u8 * data, yaffs_ExtendedTags * tags)
{
	yaffs_PackedTags2 pt;
	int localData = 0;
	void *spare = NULL;
	unsigned spareSize;
	int eccStatus; //0 = ok, 1 = fixed, -1 = unfixed

	unsigned char *bufferIn   = DevBufferIn(dev);
	unsigned char *bufferOut  = DevBufferOut(dev);
	unsigned       bufferSize = DevBufferSize(dev);

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_ReadChunkWithTagsFromNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));
	    
	if(!tags){
		spare = NULL;
		spareSize = 0;
	}else if(dev->inbandTags){
		
		if(!data) {
			localData = 1;
			data = yaffs_GetTempBuffer(dev,__LINE__);
		}
		spare = NULL;
		spareSize = 0;
	}
	else {
		spare = &pt;
		spareSize = sizeof(yaffs_PackedTags2);
	}

	yramsim_ReadPage(chunkInNAND,
					 data,data ? dev->totalBytesPerChunk : 0,
					 spare,spareSize,
					 &eccStatus);



	if(dev->inbandTags){
		if(tags){
			yaffs_PackedTags2TagsPart * pt2tp;
			pt2tp = (yaffs_PackedTags2TagsPart *)&data[dev->nDataBytesPerChunk];	
			yaffs_UnpackTags2TagsPart(tags,pt2tp);
		}
	}
	else {
		if (tags){
			yaffs_UnpackTags2(tags, &pt);
		}
	}

	if(tags && tags->chunkUsed){
		if(eccStatus == 0)
			tags->eccResult = YAFFS_ECC_RESULT_NO_ERROR;
		else if(eccStatus < 0)
			tags->eccResult = YAFFS_ECC_RESULT_UNFIXED;
		else
			tags->eccResult = YAFFS_ECC_RESULT_FIXED;
	}

	if(localData)
		yaffs_ReleaseTempBuffer(dev,data,__LINE__);
	
	return YAFFS_OK;
}

int ynandif_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockId)
{

	yramsim_MarkBlockBad(blockId);

	return YAFFS_OK;
}

int ynandif_EraseBlockInNAND(struct yaffs_DeviceStruct *dev, int blockId)
{

	yramsim_EraseBlock(blockId);

	return YAFFS_OK;
}


static int ynandif_IsBlockOk(struct yaffs_DeviceStruct *dev, int blockId)
{
	return yramsim_CheckBlockOk(blockId);
}

int ynandif_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockId, yaffs_BlockState *state, __u32 *sequenceNumber)
{
	unsigned chunkNo;
	yaffs_ExtendedTags tags;

	*sequenceNumber = 0;
	
	chunkNo = blockId * dev->nChunksPerBlock;
	
	if(!ynandif_IsBlockOk(dev,blockId)){
		*state = YAFFS_BLOCK_STATE_DEAD;
	} 
	else 
	{
		ynandif_ReadChunkWithTagsFromNAND(dev,chunkNo,NULL,&tags);

		if(!tags.chunkUsed)
		{
			*state = YAFFS_BLOCK_STATE_EMPTY;
		}
		else 
		{
			*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
			*sequenceNumber = tags.sequenceNumber;
		}
	}

	return YAFFS_OK;
}


int ynandif_GetGeometry(yaffs_Device *dev, ynandif_Geometry *geometry)
{

	yramsim_Geometry g;

	yramsim_GetGeometry(&g);
	geometry->startBlock = g.startBlock;
	geometry->endBlock = g.endBlock;
	geometry->dataSize = g.dataSize;
	geometry->spareSize = g.spareSize;
	geometry->pagesPerBlock = g.pagesPerBlock;
	geometry->hasECC = g.hasECC;
	geometry->inbandTags = g.inbandTags;
	geometry->useYaffs2 = g.useYaffs2;

	return YAFFS_OK;

}

int ynandif_InitialiseNAND(yaffs_Device *dev)
{

	yramsim_Initialise();

	return YAFFS_OK;
}
