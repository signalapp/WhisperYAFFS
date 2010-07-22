// NAND Simulator for testing YAFFS

#include <string.h>

#include "yramsim.h"

#include "yaffs_nandif.h"


#define DATA_SIZE	2048
#define SPARE_SIZE	64
#define PAGE_SIZE	(DATA_SIZE + SPARE_SIZE)
#define PAGES_PER_BLOCK	64


typedef struct {
	unsigned char page[PAGES_PER_BLOCK][PAGE_SIZE];
	unsigned blockOk;
} Block;

typedef struct {
	Block **blockList;
	int nBlocks;
} SymData;


static SymData *DevToSym(yaffs_Device *dev)
{
	ynandif_Geometry *geom = (ynandif_Geometry *)(dev->driverContext);
	SymData * sym = (SymData*)(geom->privateData);
	return sym;
} 


static void CheckInitialised(void)
{

}

static int yramsim_EraseBlockInternal(SymData *sym, unsigned blockId,int force)
{
	if(blockId < 0 || blockId >= sym->nBlocks){
		return 0;
	}

	if(!sym->blockList[blockId]){
		return 0;
	}

	if(!force && !sym->blockList[blockId]->blockOk){
		return 0;
	}

	memset(sym->blockList[blockId],0xff,sizeof(Block));
	sym->blockList[blockId]->blockOk = 1;

	return 1;
}




static int yramsim_Initialise(yaffs_Device *dev)
{
	SymData *sym = DevToSym(dev);
	Block **blockList = sym->blockList;
	return blockList != NULL;
}


static int yramsim_Deinitialise(yaffs_Device *dev)
{
	return 1;
}

static int yramsim_ReadChunk (yaffs_Device *dev, unsigned pageId, 
					  unsigned char *data, unsigned dataLength,
					  unsigned char *spare, unsigned spareLength,
					  int *eccStatus)
{
	SymData *sym = DevToSym(dev);
	Block **blockList = sym->blockList;

	unsigned blockId = pageId / PAGES_PER_BLOCK;
	unsigned pageOffset = pageId % PAGES_PER_BLOCK;
	unsigned char * d;
	unsigned char *s;
	if(blockId >= sym->nBlocks ||
	   pageOffset >= PAGES_PER_BLOCK ||
	   dataLength >DATA_SIZE ||
	   spareLength > SPARE_SIZE ||
	   !eccStatus ||
	   !blockList[blockId]->blockOk){
		   return 0;
	}

	d = blockList[blockId]->page[pageOffset];
	s = d + DATA_SIZE;

	if(data)
		memcpy(data,d,dataLength);

	if(spare)
		memcpy(spare,s,spareLength);

	*eccStatus = 0; // 0 = no error, -1 = unfixable error, 1 = fixable

	return 1;
	
}

static int yramsim_WriteChunk (yaffs_Device *dev,unsigned pageId, 
					   const unsigned char *data, unsigned dataLength,
					   const unsigned char *spare, unsigned spareLength)
{
	SymData *sym = DevToSym(dev);
	Block **blockList = sym->blockList;

	unsigned blockId = pageId / PAGES_PER_BLOCK;
	unsigned pageOffset = pageId % PAGES_PER_BLOCK;
	unsigned char * d;
	unsigned char *s;
	if(blockId >= sym->nBlocks ||
	   pageOffset >= PAGES_PER_BLOCK ||
	   dataLength >DATA_SIZE ||
	   spareLength > SPARE_SIZE ||
	   !blockList[blockId]->blockOk){
		   return 0;
	}

	d = blockList[blockId]->page[pageOffset];
	s = d + DATA_SIZE;

	if(data)
		memcpy(d,data,dataLength);

	if(spare)
		memcpy(s,spare,spareLength);

	return 1;
	
}


static int yramsim_EraseBlock(yaffs_Device *dev,unsigned blockId)
{
	SymData *sym = DevToSym(dev);

	CheckInitialised();
	return yramsim_EraseBlockInternal(sym,blockId,0);
}

static int yramsim_CheckBlockOk(yaffs_Device *dev,unsigned blockId)
{
	SymData *sym = DevToSym(dev);
	Block **blockList = sym->blockList;
	if(blockId >= sym->nBlocks){
		return 0;
	}

	return blockList[blockId]->blockOk ? 1 : 0;
}

static int yramsim_MarkBlockBad(yaffs_Device *dev,unsigned blockId)
{
	SymData *sym = DevToSym(dev);
	Block **blockList = sym->blockList;
	if(blockId >= sym->nBlocks){
		return 0;
	}

	blockList[blockId]->blockOk = 0;

	return 1;
}


static SymData *yramsim_AllocSymData(int nBlocks)
{
	int ok = 1;

	Block **blockList;
	SymData *sym;
	Block *b;
	int i;

	sym = malloc(sizeof (SymData));
	if(!sym)
		return NULL;

	blockList = malloc(nBlocks * sizeof(Block *));
	
	sym->blockList = blockList;
	sym->nBlocks = nBlocks;
	if(!blockList){
		free(sym);
		return NULL;
	}

	for(i = 0; i < nBlocks; i++)
		blockList[i] = NULL;

	for(i = 0; i < nBlocks && ok; i++){
		b=  malloc(sizeof(Block));
		if(b){
			blockList[i] = b;
			yramsim_EraseBlockInternal(sym,i,1);
		}
		else
			ok = 0;
	}

	if(!ok){
		for(i = 0; i < nBlocks; i++)
			if(blockList[i]){
				free(blockList[i]);
				blockList[i] = NULL;
			}
		free(blockList);
		blockList = NULL;
		free(sym);
		sym = NULL;
	}

	return sym;
}


struct yaffs_DeviceStruct *yramsim_CreateSim(const YCHAR *name,int nBlocks)
{
	void *sym = (void *)yramsim_AllocSymData(nBlocks);
	ynandif_Geometry *g;

	g = YMALLOC(sizeof(ynandif_Geometry));
	
	if(!sym || !g){
		if(sym)
			YFREE(sym);
		if(g)
			YFREE(g);
		return NULL;
	}

	memset(g,0,sizeof(ynandif_Geometry));
	g->startBlock = 0;
	g->endBlock = nBlocks - 1;
	g->dataSize = DATA_SIZE;
	g->spareSize= SPARE_SIZE;
	g->pagesPerBlock = PAGES_PER_BLOCK;
	g->hasECC = 1;
	g->inbandTags = 0;
	g->useYaffs2 = 1;
	g->initialise = yramsim_Initialise;
	g->deinitialise = yramsim_Deinitialise; 
	g->readChunk = yramsim_ReadChunk, 
	g->writeChunk = yramsim_WriteChunk,
	g->eraseBlock = yramsim_EraseBlock,
	g->checkBlockOk = yramsim_CheckBlockOk,
	g->markBlockBad = yramsim_MarkBlockBad,
	g->privateData = sym;

	return yaffs_AddDeviceFromGeometry(name,g);
}
