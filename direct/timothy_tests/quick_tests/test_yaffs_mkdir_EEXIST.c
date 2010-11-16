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

#include "test_yaffs_mkdir_EEXIST.h"

static int output = -1;

int test_yaffs_mkdir_EEXIST(void)
{
	int error_code = 0;
	
	if (0==yaffs_access(DIR_PATH,0)){
		/* if the file exists  then the file does not need to be created the first time*/
		
	} else {
	
		output = yaffs_mkdir(DIR_PATH,O_CREAT | O_RDWR);
		if (output <0) {
			print_message("failed to create the directory the first time\n",2);
			return -1;
		}
	}

	output = yaffs_mkdir("/yaffs2/new_directory/",O_CREAT | O_RDWR);
	if (output < 0){
		error_code = yaffs_get_error();
		if (abs(error_code) == EEXIST){
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


int test_yaffs_mkdir_EEXIST_clean(void)
{
	if (output >= 0){
		return yaffs_rmdir(DIR_PATH);
	} else {
		return 1;	
	}
}

