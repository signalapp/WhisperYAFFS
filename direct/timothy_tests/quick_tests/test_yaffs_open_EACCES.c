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

#include "test_yaffs_open_EACCES.h"

static int handle = -1;

int test_yaffs_open_EACCES(void)
{
	int error_code=0;
	int output =-1;
	if (yaffs_access(FILE_PATH,0)!=0){
		output=yaffs_close(test_yaffs_open());
		if (output<0){
			print_message("failed to open file\n",2);
			return -1;
		}
	}

	output=yaffs_chmod(FILE_PATH,S_IREAD);
	if (output<0){
		print_message("failed to chmod file\n",2);
		return -1;
	}
	handle=yaffs_open(FILE_PATH, O_TRUNC| O_RDWR,FILE_MODE );
	if (handle <0){
		error_code=yaffs_get_error();
		if (abs(error_code)==EACCES){
			return 1;
		}
		else {
			print_message("different error than expected\n",2);
			return -1;
		}
	}
	else {
		print_message("file opened with read and write permissions, chmoded to read only.(which is a bad thing)\n",2);
		return -1;
	}
}

int test_yaffs_open_EACCES_clean(void)
{
	int output= -1;
	int output2=-1;	
	if (handle >=0){
		output2= yaffs_close(handle);
	}
	output=yaffs_chmod(FILE_PATH,S_IREAD|S_IWRITE);
	if (output<0){
		print_message("failed to chmod file\n",2);
		return -1;
	}
	return (output2&&output);
	
}

