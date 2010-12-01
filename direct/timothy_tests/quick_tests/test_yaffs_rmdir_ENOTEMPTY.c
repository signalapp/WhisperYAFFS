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

#include "test_yaffs_rmdir_ENOTEMPTY.h"


int test_yaffs_rmdir_ENOTEMPTY(void)
{
	int output=0;
	int error_code =0;

	if (0 !=  yaffs_access(DIR_PATH,0)) {
		output = yaffs_mkdir(DIR_PATH,S_IWRITE | S_IREAD);
		if (output < 0) {
			print_message("failed to create directory\n",2);
			return -1;
		}
	}
	if (0 !=  yaffs_access("/yaffs2/new_directory/dir",0)) {
		output = yaffs_mkdir("/yaffs2/new_directory/dir",S_IWRITE | S_IREAD);
		if (output < 0) {
			print_message("failed to create directory\n",2);
			return -1;
		}
	}
	output = yaffs_rmdir(DIR_PATH);
	if (output<0){ 
		error_code=yaffs_get_error();
		if (abs(error_code)==ENOTEMPTY){
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


int test_yaffs_rmdir_ENOTEMPTY_clean(void)
{
	int output = -1;
	int output2 = -1;
	if (0 ==  yaffs_access("/yaffs2/new_directory/dir",0)) {
		output = yaffs_rmdir("/yaffs2/new_directory/dir");
		if (output < 0) {
			print_message("failed to remove the directory\n",2);
			output=1;
		}
	}
	if (0 ==  yaffs_access(DIR_PATH,0)) {
		output = yaffs_rmdir(DIR_PATH);
		if (output < 0) {
			print_message("failed to remove the directory\n",2);
			output2=1;
		}
	}
	return (output && output2);

}

