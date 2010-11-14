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

#include "test_yaffs_lseek_EFBIG.h"

static int handle = -1;

int test_yaffs_lseek_EFBIG(void)
{
	handle = test_yaffs_open();
	int error_code = 0;
	int output = 0;

	if (handle < 0){
		print_message("failed to open file\n",2);
		return -1;
	}
	
	output = yaffs_lseek(handle, 100000000000000000000000000000000000000, SEEK_SET);

	if (output < 0){
		error_code = yaffs_get_error();
		if (abs(error_code) == EINVAL){
			return 1;
		} else {
			print_message("different error than expected\n", 2);
			return -1;
		}
	} else {
		print_message("lseeked to a very large size (which is a bad thing)\n", 2);
		return -1;
	}
}

int test_yaffs_lseek_EFBIG_clean(void)
{
	if (handle >= 0){
		return yaffs_close(handle);
	} else {
		return -1; /* no handle was opened so there is no need to close a handle */
	}	
}

