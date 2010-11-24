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

#include "test_yaffs_lstat_ENOENT.h"


int test_yaffs_lstat_ENOENT(void)
{
	int output = -1;
	int error_code = -1;
	struct yaffs_stat stat;
	if (0 != yaffs_access(FILE_PATH,0)){
		output=test_yaffs_open();
		if (output>=0){
			output = yaffs_close(output);
			if (output<0){
				print_message("failed to close file\n",2);
				return -1;
			}
		} else {
			print_message("failed to open file\n",2);
			return -1;
		}
	}
	if (0 != yaffs_access(SYMLINK_PATH,0)){
		output=yaffs_symlink(FILE_PATH,SYMLINK_PATH);
		if (output<0){
			print_message("failed to open file\n",2);
			return -1;
		}
	}
	
	output =yaffs_lstat("yaffs2/non-existing-dir/",&stat);
	if (output<0){
		error_code=yaffs_get_error();
		if (abs(error_code)==ENOENT){
			return 1;
		} else {
			print_message("different error than expected\n", 2);
			return -1;
		}
	} else {
		print_message("lstatted a non-existing file\n", 2);
		return -1;
	}
}


int test_yaffs_lstat_ENOENT_clean(void)
{
	return 1;
}

