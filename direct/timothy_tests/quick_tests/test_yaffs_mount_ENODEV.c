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

#include "test_yaffs_mount_ENODEV.h"



int test_yaffs_mount_ENODEV(void)
{
	int output=0;
	int error_code=0;

	output = yaffs_mount("/nonexisting_mount_point/");
	if (output<0){
		error_code = yaffs_get_error();
		if (abs(error_code) == ENODEV){
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("non existant directory opened.(which is a bad thing)\n",2);
		return -1;
	}
}

int test_yaffs_mount_ENODEV_clean(void)
{
	return 1;
}

