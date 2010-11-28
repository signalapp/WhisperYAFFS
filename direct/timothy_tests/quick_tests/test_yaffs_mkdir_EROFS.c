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

#include "test_yaffs_mkdir_EROFS.h"

static int output = -1;

int test_yaffs_mkdir_EROFS(void)
{
	int error_code = 0;

	EROFS_setup();

	output = yaffs_mkdir(DIR_PATH,O_CREAT | O_RDWR);
	if (output < 0){
		error_code = yaffs_get_error();
		if (abs(error_code) == EROFS){
			return 1;
		} else {
			print_message("different error than expected\n", 2);
			return -1;
		}
	} else {
		print_message("created a new directory with EROFS set (which is a bad thing)\n", 2);
		return -1;
	}
}


int test_yaffs_mkdir_EROFS_clean(void)
{
	int output2 = 1;
	if (output >= 0){
		output2= yaffs_rmdir(DIR_PATH);
	} 
	return (EROFS_clean() && output2);
}

