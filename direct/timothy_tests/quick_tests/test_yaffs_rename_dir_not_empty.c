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

#include "test_yaffs_rename_dir.h"


int test_yaffs_rename_dir(void)
{
	int output=0;
	int error_code =0;
	output = yaffs_open("/yaffs2/new_directory/file",O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
	if (output < 0 )

	if (0 !=  yaffs_access(FILE_PATH,0)) {
		output = test_yaffs_open();
		if (output < 0) {
			print_message("failed to create file\n",2);
			return -1;
		}
	}
	output = yaffs_rename( DIR_PATH , RENAME_DIR_PATH);
	return output;	
}


int test_yaffs_rename_dir_clean(void)
{
	int output = 0;
	if (0 ==  yaffs_access("/yaffs2/new_directory/file",0)) {
		output = yaffs_unlink("/yaffs2/new_directory/file");
		if (output < 0) {
			print_message("failed to remove file\n",2);
			return -1;
		}
	}

	if (0 ==  yaffs_access(RENAME_DIR_PATH,0)) {
		output = yaffs_rename(RENAME_DIR_PATH,DIR_PATH);
		if (output < 0) {
			print_message("failed to rename the dir\n",2);
			return -1;
		}
	}
	return 1;

}

