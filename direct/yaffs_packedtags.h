
#ifndef __YAFFS_PACKEDTAGS_H__
#define __YAFFS_PACKEDTAGS_H__

#include "yaffs_guts.h"

typedef struct
{   
	unsigned chunkId:20;
    unsigned serialNumber:2;
    unsigned byteCount:10;
    unsigned objectId:18;
    unsigned ecc:12;
    unsigned deleted:1;
    unsigned unusedStuff:1;
    unsigned shouldBeFF;

} yaffs_PackedTags;

void yaffs_PackTags(yaffs_PackedTags *pt, yaffs_ExtendedTags *t);
void yaffs_UnpackTags(yaffs_ExtendedTags *t, yaffs_PackedTags *pt);
#endif


