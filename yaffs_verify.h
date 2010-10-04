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

#ifndef __YAFFS_VERIFY_H__
#define __YAFFS_VERIFY_H__

#include "yaffs_guts.h"

void yaffs_verify_blk(yaffs_Device *dev, yaffs_BlockInfo *bi, int n);
void yaffs_verify_collected_blk(yaffs_Device *dev, yaffs_BlockInfo *bi, int n);
void yaffs_verify_blocks(yaffs_Device *dev);

void yaffs_verify_oh(yaffs_Object *obj, yaffs_ObjectHeader *oh, yaffs_ExtendedTags *tags, int parentCheck);
void yaffs_verify_file(yaffs_Object *obj);
void yaffs_verify_link(yaffs_Object *obj);
void yaffs_verify_symlink(yaffs_Object *obj);
void yaffs_verify_special(yaffs_Object *obj);
void yaffs_verify_obj(yaffs_Object *obj);
void yaffs_verify_objects(yaffs_Device *dev);
void yaffs_verify_obj_in_dir(yaffs_Object *obj);
void yaffs_verify_dir(yaffs_Object *directory);
void yaffs_verify_free_chunks(yaffs_Device *dev);

int yaffs_verify_file_sane(yaffs_Object *obj);

int yaffs_skip_verification(yaffs_Device *dev);

#endif

