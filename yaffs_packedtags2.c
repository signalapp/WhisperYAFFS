#include "yaffs_packedtags2.h"
#include "yportenv.h"


static void yaffs_DumpPackedTags2(const yaffs_PackedTags2 *pt)
{
	T(YAFFS_TRACE_MTD,(TSTR("packed tags obj %d chunk %d byte %d seq %d"TENDSTR),pt->t.objectId,pt->t.chunkId,pt->t.byteCount,pt->t.sequenceNumber));
}

static void yaffs_DumpTags2(const yaffs_ExtendedTags *t)
{
	T(YAFFS_TRACE_MTD,(TSTR("ext.tags eccres %d blkbad %d chused %d obj %d chunk%d byte %d del %d ser %d seq %d"TENDSTR),
	     t->eccResult, t->blockBad, t->chunkUsed, t->objectId, t->chunkId, t->byteCount, t->chunkDeleted, t->serialNumber, t->sequenceNumber));
	  
}

void yaffs_PackTags2(yaffs_PackedTags2 *pt, const yaffs_ExtendedTags *t)
{
	pt->t.chunkId = t->chunkId;
	pt->t.sequenceNumber = t->sequenceNumber;
	pt->t.byteCount = t->byteCount;
	pt->t.objectId = t->objectId;
	
	yaffs_DumpPackedTags2(pt);
	yaffs_DumpTags2(t);
	
	yaffs_ECCCalculateOther((unsigned char *)&pt->t,sizeof(yaffs_PackedTags2TagsPart),&pt->ecc);
	
}

void yaffs_UnpackTags2(yaffs_ExtendedTags *t, const yaffs_PackedTags2 *pt)
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

	yaffs_DumpPackedTags2(pt);
	yaffs_DumpTags2(t);

}

