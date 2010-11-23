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

#include "test_yaffs_symlink.h"

static int output = 0;

int test_yaffs_symlink(void)
{
	if (0 == yaffs_access(SYMLINK_PATH,0)){
		output=yaffs_unlink(SYMLINK_PATH);
		if (output<0){
			print_message("failed to unlink symlink\n",2);
			return -1;
		}
	}
	output = yaffs_symlink(FILE_PATH,SYMLINK_PATH);
	return output;
}


int test_yaffs_symlink_clean(void)
{
	if (output >= 0){
		return yaffs_unlink(SYMLINK_PATH);
	} else {
		return 1;	/* the file failed to open so there is no need to close it */
	}
}

