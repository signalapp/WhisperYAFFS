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

#include "test_yaffs_mknode.h"

static int output = 0;

int test_yaffs_mknode(void)
{
	output = yaffs_mknod(NODE_PATH,S_IREAD | S_IWRITE);
	return output;
}


int test_yaffs_mknode_clean(void)
{
	if (output >= 0){
		return yaffs_unlink(NODE_PATH);
	} else {
		return 1;	/* the file failed to open so there is no need to close it */
	}
}

