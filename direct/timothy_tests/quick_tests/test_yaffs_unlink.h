/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __test_yaffs_unlink_h__
#define __test_yaffs_unlink_h__

#include "lib.h"
#include "yaffsfs.h"
#include "test_yaffs_open.h"
#include "test_yaffs_access.h"

int test_yaffs_unlink(void);
int test_yaffs_unlink_clean(void);

#endif
