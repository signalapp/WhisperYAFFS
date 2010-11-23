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

#include "test_yaffs_rename_file_to_dir.h"


int test_yaffs_rename_file_to_dir(void)
{
	int output=0;

	if (0 !=  yaffs_access(FILE_PATH,0)) {
		output = test_yaffs_open();
		if (output < 0) {
			print_message("failed to create file\n",2);
			return -1;
		} else {
			output = yaffs_close(output);
			if (output < 0) {
				print_message("failed to close file\n",2);
				return -1;
			}
		}
	}
	output = yaffs_rename( "/yaffs2/foo" , RENAME_DIR_PATH);
	if (output<0){ 
		print_message("failed to rename a file over an empty directory\n",2);
		return -1;
	} else{
		return 1;
	}	

}


int test_yaffs_rename_file_to_dir_clean(void)
{
	int output = 0;
	if (0 ==  yaffs_access(RENAME_DIR_PATH,0)) {
		output = yaffs_unlink(RENAME_DIR_PATH);
		if (output < 0) {
			print_message("failed to unlink the file\n",2);
			return -1;
		}
	}
	return 1;

}

