#include "yaffs_packedtags2.h"
#include "yportenv.h"



void yaffs_PackTags2(yaffs_PackedTags2 *pt, yaffs_ExtendedTags *t)
{
	pt->t.chunkId = t->chunkId;
	pt->t.sequenceNumber = t->sequenceNumber;
	pt->t.byteCount = t->byteCount;
	pt->t.objectId = t->objectId;
	
	yaffs_ECCCalculateOther((unsigned char *)&pt->t,sizeof(yaffs_PackedTags2TagsPart),&pt->ecc);
	
}

void yaffs_UnpackTags2(yaffs_ExtendedTags *t, yaffs_PackedTags2 *pt)
{

	
	if(pt->t.sequenceNumber == 0xFFFFFFFF)
	{
		memset(t,0,sizeof(yaffs_ExtendedTags));
		
	}
	else
	{
		// Page is in use
		yaffs_ECCOther ecc;
		yaffs_ECCCalculateOther((unsigned char *)&pt->t,sizeof(yaffs_PackedTags2TagsPart),&ecc);
		t->eccResult = yaffs_ECCCorrectOther((unsigned char *)&pt->t,sizeof(yaffs_PackedTags2TagsPart),&pt->ecc,&ecc);
		t->blockBad = 0;
		t->chunkUsed = 1;
		t->objectId = pt->t.objectId;
		t->chunkId =  pt->t.chunkId;
		t->byteCount = pt->t.byteCount;
		t->chunkDeleted = 0;
		t->serialNumber = 0;
		t->sequenceNumber = pt->t.sequenceNumber;
	}
}

