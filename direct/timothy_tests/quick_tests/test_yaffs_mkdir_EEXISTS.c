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

#include "test_yaffs_mkdir_EEXISTS.h"

static int output = -1;

int test_yaffs_mkdir_EEXISTS(void)
{
	output = yaffs_mkdir("/yaffs2/new_directory/",O_CREAT | O_RDWR);
	if (output <0) {
		print_message("filed to create the directory the first time\n",2);
		return -1;
	}
	output = yaffs_mkdir("/yaffs2/new_directory/",O_CREAT | O_RDWR);
	if (output < 0){
		error_code = yaffs_get_error();
		if (abs(error_code) == EEXISTS){
			return 1;
		} else {
			print_message("different error than expected\n", 2);
			return -1;
		}
	} else {
		print_message("lseeked to a negative position (which is a bad thing)\n", 2);
		return -1;
	}
}


int test_yaffs_mkdir_EEXISTS_clean(void)
{
	if (output >= 0){
		return yaffs_rmdir("/yaffs2/new_directory/");
	} else {
		return 1;	
	}
}

