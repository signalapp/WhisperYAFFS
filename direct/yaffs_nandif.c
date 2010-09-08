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

#include "yramsim.h"

#include "yaffs_trace.h"
#include "yaffsfs.h"


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
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_WriteChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));
	    

	/* For yaffs2 writing there must be both data and tags.
	 * If we're using inband tags, then the tags are stuffed into
	 * the end of the data buffer.
	 */

	if(dev->param.inbandTags){
		yaffs_PackedTags2TagsPart *pt2tp;
		pt2tp = (yaffs_PackedTags2TagsPart *)(data + dev->nDataBytesPerChunk);
		yaffs_PackTags2TagsPart(pt2tp,tags);
		spare = NULL;
		spareSize = 0;
	}
	else{
		yaffs_PackTags2(&pt, tags,!dev->param.noTagsECC);
		spare = &pt;
		spareSize = sizeof(yaffs_PackedTags2);
	}
	
	retval = geometry->writeChunk(dev,chunkInNAND,
					  data, dev->param.totalBytesPerChunk, spare, spareSize);

	return retval;
}

int ynandif_ReadChunkWithTagsFromNAND(yaffs_Device * dev, int chunkInNAND,
				       __u8 * data, yaffs_ExtendedTags * tags)
{
	yaffs_PackedTags2 pt;
	int localData = 0;
	void *spare = NULL;
	unsigned spareSize;
	int retval = 0;
	int eccStatus; //0 = ok, 1 = fixed, -1 = unfixed
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_ReadChunkWithTagsFromNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));
	    
	if(!tags){
		spare = NULL;
		spareSize = 0;
	}else if(dev->param.inbandTags){
		
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

	retval = geometry->readChunk(dev,chunkInNAND,
					 data,
					 data ? dev->param.totalBytesPerChunk : 0,
					 spare,spareSize,
					 &eccStatus);

	if(dev->param.inbandTags){
		if(tags){
			yaffs_PackedTags2TagsPart * pt2tp;
			pt2tp = (yaffs_PackedTags2TagsPart *)&data[dev->nDataBytesPerChunk];	
			yaffs_UnpackTags2TagsPart(tags,pt2tp);
		}
	}
	else {
		if (tags){
			yaffs_UnpackTags2(tags, &pt,!dev->param.noTagsECC);
		}
	}

	if(tags && tags->chunkUsed){
		if(eccStatus < 0 || 
		   tags->eccResult == YAFFS_ECC_RESULT_UNFIXED)
			tags->eccResult = YAFFS_ECC_RESULT_UNFIXED;
		else if(eccStatus > 0 ||
			     tags->eccResult == YAFFS_ECC_RESULT_FIXED)
			tags->eccResult = YAFFS_ECC_RESULT_FIXED;
		else
			tags->eccResult = YAFFS_ECC_RESULT_NO_ERROR;
	}

	if(localData)
		yaffs_ReleaseTempBuffer(dev,data,__LINE__);
	
	return retval;
}

int ynandif_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockId)
{
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	return geometry->markBlockBad(dev,blockId);
}

int ynandif_EraseBlockInNAND(struct yaffs_DeviceStruct *dev, int blockId)
{
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	return geometry->eraseBlock(dev,blockId);

}


static int ynandif_IsBlockOk(struct yaffs_DeviceStruct *dev, int blockId)
{
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	return geometry->checkBlockOk(dev,blockId);
}

int ynandif_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockId, yaffs_BlockState *state, __u32 *sequenceNumber)
{
	unsigned chunkNo;
	yaffs_ExtendedTags tags;

	*sequenceNumber = 0;
	
	chunkNo = blockId * dev->param.nChunksPerBlock;
	
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


int ynandif_InitialiseNAND(yaffs_Device *dev)
{
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	geometry->initialise(dev);

	return YAFFS_OK;
}

int ynandif_DeinitialiseNAND(yaffs_Device *dev)
{
	ynandif_Geometry *geometry = (ynandif_Geometry *)(dev->driverContext);

	geometry->deinitialise(dev);

	return YAFFS_OK;
}


struct yaffs_DeviceStruct * 
	yaffs_AddDeviceFromGeometry(const YCHAR *name,
					const ynandif_Geometry *geometry)
{
	YCHAR *clonedName = YMALLOC(sizeof(YCHAR) * (yaffs_strnlen(name,YAFFS_MAX_NAME_LENGTH)+1));
	struct yaffs_DeviceStruct *dev = YMALLOC(sizeof(struct yaffs_DeviceStruct));

	if(dev && clonedName){
		memset(dev,0,sizeof(struct yaffs_DeviceStruct));
		yaffs_strcpy(clonedName,name);

		dev->param.name = clonedName;
		dev->param.writeChunkWithTagsToNAND  = ynandif_WriteChunkWithTagsToNAND;
		dev->param.readChunkWithTagsFromNAND = ynandif_ReadChunkWithTagsFromNAND;
		dev->param.eraseBlockInNAND          = ynandif_EraseBlockInNAND;
		dev->param.initialiseNAND            = ynandif_InitialiseNAND;
		dev->param.queryNANDBlock            = ynandif_QueryNANDBlock;
		dev->param.markNANDBlockBad          = ynandif_MarkNANDBlockBad;
		dev->param.nShortOpCaches			   = 20;
		dev->param.startBlock                = geometry->startBlock;
		dev->param.endBlock                  = geometry->endBlock;
		dev->param.totalBytesPerChunk		   = geometry->dataSize;
		dev->param.spareBytesPerChunk		   = geometry->spareSize;
		dev->param.inbandTags				   = geometry->inbandTags;
		dev->param.nChunksPerBlock		   = geometry->pagesPerBlock;
		dev->param.useNANDECC				   = geometry->hasECC;
		dev->param.isYaffs2				   = geometry->useYaffs2;
		dev->param.nReservedBlocks		   = 5;
		dev->driverContext			   = (void *)geometry;

		yaffs_AddDevice(dev);

		return dev;
	}

	if(dev)
		YFREE(dev);
	if(clonedName)
		YFREE(clonedName);

	return NULL;
}
