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
} SimData;


SimData *simDevs[N_RAM_SIM_DEVS];

static SimData *DevToSim(yaffs_Device *dev)
{
	ynandif_Geometry *geom = (ynandif_Geometry *)(dev->driverContext);
	SimData * sim = (SimData*)(geom->privateData);
	return sim;
}


static void CheckInitialised(void)
{

}

static int yramsim_EraseBlockInternal(SimData *sim, unsigned blockId,int force)
{
	if(blockId < 0 || blockId >= sim->nBlocks){
		return 0;
	}

	if(!sim->blockList[blockId]){
		return 0;
	}

	if(!force && !sim->blockList[blockId]->blockOk){
		return 0;
	}

	memset(sim->blockList[blockId],0xff,sizeof(Block));
	sim->blockList[blockId]->blockOk = 1;

	return 1;
}




static int yramsim_Initialise(yaffs_Device *dev)
{
	SimData *sim = DevToSim(dev);
	Block **blockList = sim->blockList;
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
	SimData *sim = DevToSim(dev);
	Block **blockList = sim->blockList;

	unsigned blockId = pageId / PAGES_PER_BLOCK;
	unsigned pageOffset = pageId % PAGES_PER_BLOCK;
	unsigned char * d;
	unsigned char *s;
	if(blockId >= sim->nBlocks ||
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
	SimData *sim = DevToSim(dev);
	Block **blockList = sim->blockList;

	unsigned blockId = pageId / PAGES_PER_BLOCK;
	unsigned pageOffset = pageId % PAGES_PER_BLOCK;
	unsigned char * d;
	unsigned char *s;
	if(blockId >= sim->nBlocks ||
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
	SimData *sim = DevToSim(dev);

	CheckInitialised();
	return yramsim_EraseBlockInternal(sim,blockId,0);
}

static int yramsim_CheckBlockOk(yaffs_Device *dev,unsigned blockId)
{
	SimData *sim = DevToSim(dev);
	Block **blockList = sim->blockList;
	if(blockId >= sim->nBlocks){
		return 0;
	}

	return blockList[blockId]->blockOk ? 1 : 0;
}

static int yramsim_MarkBlockBad(yaffs_Device *dev,unsigned blockId)
{
	SimData *sim = DevToSim(dev);
	Block **blockList = sim->blockList;
	if(blockId >= sim->nBlocks){
		return 0;
	}

	blockList[blockId]->blockOk = 0;

	return 1;
}


static SimData *yramsim_AllocSimData(__u32 devId, __u32 nBlocks)
{
	int ok = 1;

	Block **blockList;
	SimData *sim;
	Block *b;
	__u32 i;

	if(devId >= N_RAM_SIM_DEVS)
		return NULL;

	sim = simDevs[devId];

	if(sim)
		return sim;

	sim = malloc(sizeof (SimData));
	if(!sim)
		return NULL;

	simDevs[devId] = sim;

	blockList = malloc(nBlocks * sizeof(Block *));

	sim->blockList = blockList;
	sim->nBlocks = nBlocks;
	if(!blockList){
		free(sim);
		return NULL;
	}

	for(i = 0; i < nBlocks; i++)
		blockList[i] = NULL;

	for(i = 0; i < nBlocks && ok; i++){
		b=  malloc(sizeof(Block));
		if(b){
			blockList[i] = b;
			yramsim_EraseBlockInternal(sim,i,1);
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
		free(sim);
		sim = NULL;
	}

	return sim;
}


struct yaffs_DeviceStruct *yramsim_CreateRamSim(const YCHAR *name,
				__u32 devId, __u32 nBlocks,
				__u32 startBlock, __u32 endBlock)
{
	SimData *sim;
	ynandif_Geometry *g;

	sim = yramsim_AllocSimData(devId, nBlocks);

	g = YMALLOC(sizeof(ynandif_Geometry));

	if(!sim || !g){
		if(g)
			YFREE(g);
		return NULL;
	}

	if(startBlock >= sim->nBlocks)
		startBlock = 0;
	if(endBlock == 0 || endBlock >= sim->nBlocks)
		endBlock = sim->nBlocks - 1;

	memset(g,0,sizeof(ynandif_Geometry));
	g->startBlock = startBlock;
	g->endBlock = endBlock;
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
	g->privateData = (void *)sim;

	return yaffs_add_dev_from_geometry(name,g);
}
