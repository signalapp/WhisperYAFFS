/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_TAGSCOMPAT_H__
#define __YAFFS_TAGSCOMPAT_H__

#include "yaffs_guts.h"
int yaffs_tags_compat_wr(yaffs_Device *dev,
						int chunkInNAND,
						const __u8 *data,
						const yaffs_ExtendedTags *tags);
int yaffs_tags_compat_rd(yaffs_Device *dev,
						int chunkInNAND,
						__u8 *data,
						yaffs_ExtendedTags *tags);
int yaffs_tags_compat_mark_bad(struct yaffs_DeviceStruct *dev,
					    int blockNo);
int yaffs_tags_compat_query_block(struct yaffs_DeviceStruct *dev,
					  int blockNo,
					  yaffs_BlockState *state,
					  __u32 *sequenceNumber);

void yaffs_calc_tags_ecc(yaffs_Tags *tags);
int yaffs_check_tags_ecc(yaffs_Tags *tags);
int yaffs_count_bits(__u8 byte);

#endif
