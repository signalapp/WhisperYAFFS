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

#include "test_yaffs_rename.h"


int test_yaffs_rename(void)
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
	output = yaffs_rename( FILE_PATH , RENAME_PATH);
	return output;	
}


int test_yaffs_rename_clean(void)
{
	int output = 0;
	if (0 ==  yaffs_access(FILE_PATH,0)) {
		output = test_yaffs_open();
		if (output < 0) {
			print_message("failed to open a new file\n",2);
			return -1;
		}
	}
	return 1;

}

