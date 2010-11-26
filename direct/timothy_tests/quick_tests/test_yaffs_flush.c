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
	int output=-1;
	handle = test_yaffs_open();
	if (handle >= 0){
		output =yaffs_flush(handle);
	} else {
		print_message("failed to open file\n",2);
	}
	return output;
}


int test_yaffs_flush_clean(void)
{
	if (handle>=0){
		return yaffs_close(handle);
	} else {
		return -1;
	}
}

