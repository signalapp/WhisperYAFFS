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

#include "test_yaffs_flush.h"

static int handle =-1;

int test_yaffs_flush(void)
{
	int output=0;
	handle = test_yaffs_open();
	output =yaffs_flush(handle);
	
}


int test_yaffs_flush_clean(void)
{
	return yaffs_close(handle);
}

