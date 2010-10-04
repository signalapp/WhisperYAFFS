/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __YAFFS_YAFFS2_H__
#define __YAFFS_YAFFS2_H__

#include "yaffs_guts.h"

void yaffs_calc_oldest_dirty_seq(yaffs_Device *dev);
void yaffs2_find_oldest_dirty_seq(yaffs_Device *dev);
void yaffs2_clear_oldest_dirty_seq(yaffs_Device *dev, yaffs_BlockInfo *bi);
void yaffs2_update_oldest_dirty_seq(yaffs_Device *dev, unsigned blockNo, yaffs_BlockInfo *bi);
int yaffs_block_ok_for_gc(yaffs_Device *dev, yaffs_BlockInfo *bi);
__u32 yaffs2_find_refresh_block(yaffs_Device *dev);
int yaffs2_checkpt_required(yaffs_Device *dev);
int yaffs_calc_checkpt_blocks_required(yaffs_Device *dev);


void yaffs2_checkpt_invalidate(yaffs_Device *dev);
int yaffs2_checkpt_save(yaffs_Device *dev);
int yaffs2_checkpt_restore(yaffs_Device *dev);

int yaffs2_handle_hole(yaffs_Object *obj, loff_t newSize);
int yaffs2_scan_backwards(yaffs_Device *dev);

#endif
