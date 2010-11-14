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

#include "test_yaffs_ftruncate_EFBIG.h"

static int handle = -1;

int test_yaffs_ftruncate_EFBIG(void)
{
	int output = 0;
	int error_code = 0;
	handle = test_yaffs_open();

	if (handle >= 0){
		output = yaffs_ftruncate(handle,10000000000000000000000000000000);
		if (output < 0){
			error_code = yaffs_get_error();
			if (abs(error_code) == EINVAL){	/* yaffs uses the error EINVAL instead of EFBIG */
				return 1;
			} else {
				print_message("different error than expected\n", 2);
				return -1;
			}
		} else {
			print_message("file truncated to a very large size.(which is a bad thing)\n", 2);
			return -1;
		}
	} else {
		print_message("error opening file\n", 2);
		return -1;
	}
}

int test_yaffs_ftruncate_EFBIG_clean(void){
	/* change file size back to orignal size */
	int output = 0;
	if (handle >= 0){
		output = yaffs_ftruncate(handle,FILE_SIZE );
		if (output >= 0){
			return yaffs_close(handle);
		} else {
			print_message("failed to truncate file\n", 2);
			return -1;
		}
	} else {
		/* the file was not opened so the file could not be truncated */
		return 1;
	}
}
