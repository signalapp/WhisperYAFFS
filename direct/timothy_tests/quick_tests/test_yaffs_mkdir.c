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

#include "test_yaffs_mkdir.h"



int test_yaffs_mkdir(void)
{
	int output=-1;
	if (0==yaffs_access(DIR_PATH,0)) {
		output=yaffs_rmdir(DIR_PATH);
		if (output<0) {
			print_message("the directory already exists and cannot be removed.\n",2);
			return -1;
		}
	}
	output = yaffs_mkdir(DIR_PATH,O_CREAT | O_RDWR);
	return output;
}


int test_yaffs_mkdir_clean(void)
{
	if (0==yaffs_access(DIR_PATH,0)) {
		return yaffs_rmdir(DIR_PATH);
	} else {
		return 1;	
	}
}

