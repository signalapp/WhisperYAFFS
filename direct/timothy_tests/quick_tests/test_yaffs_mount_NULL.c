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

#include "test_yaffs_mount_NULL.h"



int test_yaffs_mount_NULL(void)
{
	int output=0;
	int error_code=0;

	output = yaffs_mount(NULL);
	if (output<0){
		error_code = yaffs_get_error();
		if (abs(error_code) == EFAULT){
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("mounted a null mount point.(which is a bad thing)\n",2);
		return -1;
	}
}

int test_yaffs_mount_NULL_clean(void)
{
	return 1;
}

