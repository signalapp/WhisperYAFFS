// This is used to pack YAFFS2 tags, not YAFFS1tags.

#ifndef __YAFFS_PACKEDTAGS2_H__
#define __YAFFS_PACKEDTAGS2_H__


#include "yaffs_guts.h"
#include "yaffs_ecc.h"

typedef struct
{   
    unsigned sequenceNumber;
    unsigned chunkId;
    unsigned objectId;
    unsigned byteCount;
    unsigned nandTagsEcc;
    
} yaffs_PackedTags2TagsPart;

typedef struct
{
    yaffs_PackedTags2TagsPart t;
    yaffs_ECCOther ecc;
} yaffs_PackedTags2;


void yaffs_PackTags2(yaffs_PackedTags2 *pt, const  yaffs_ExtendedTags *t);
void yaffs_UnpackTags2(yaffs_ExtendedTags *t, const yaffs_PackedTags2 *pt);
#endif


