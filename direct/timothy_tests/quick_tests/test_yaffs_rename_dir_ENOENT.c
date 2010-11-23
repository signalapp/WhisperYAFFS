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

#include "test_yaffs_rename_dir_ENOENT.h"


int test_yaffs_rename_dir_ENOENT(void)
{
	int output=0;
	int error_code =0;

	if (0 !=  yaffs_access(FILE_PATH,0)) {
		output = test_yaffs_open();
		if (output < 0) {
			print_message("failed to create file\n",2);
			return -1;
		}
	}
	output = yaffs_rename( "/yaffs2/non-existing-dir/" , RENAME_DIR_PATH);
	if (output<0){ 
		error_code=yaffs_get_error();
		if (abs(error_code)==EINVAL){
			return 1;
		} else {
			print_message("returned error does not match the the expected error\n",2);
			return -1;
		}
	} else{
		print_message("removed /yaffs2/ directory (which is a bad thing)\n",2);
		return -1;
	}	

}


int test_yaffs_rename_dir_ENOENT_clean(void)
{
	int output = 0;
	if (0 ==  yaffs_access(RENAME_DIR_PATH,0)) {
		output = yaffs_rename(RENAME_DIR_PATH,DIR_PATH);
		if (output < 0) {
			print_message("failed to rename the file\n",2);
			return -1;
		}
	}
	return 1;

}

