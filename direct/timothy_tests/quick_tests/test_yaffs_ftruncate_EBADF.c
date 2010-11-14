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

#include "test_yaffs_ftruncate_EBADF.h"

static int handle = 0;

int test_yaffs_ftruncate_EBADF(void)
{
	int output = 0;
	int error_code = 0;
	output = yaffs_ftruncate(-1,FILE_SIZE_TRUNCATED );
	if (output < 0){
		error_code=yaffs_get_error();
		//printf("EISDIR def %d, Error code %d\n", EISDIR,error_code);
		if (abs(error_code) == EBADF){
			return 1;
		} else {
			print_message("different error than expected\n", 2);
			return -1;
		}
	} else {
		print_message("non existant file truncated.(which is a bad thing)\n", 2);
		return -1;
	}
}

int test_yaffs_ftruncate_EBADF_clean(void)
{
	return 1;
}
