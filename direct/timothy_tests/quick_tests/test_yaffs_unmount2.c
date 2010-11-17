/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "test_yaffs_unmount2.h"

int test_yaffs_unmount2(void)
{
	int output = -1;

	output = yaffs_unmount2(YAFFS_MOUNT_POINT,1);
	return output;
}

int test_yaffs_unmount2_clean(void)
{
	return test_yaffs_mount();	
}
