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

#include "test_yaffs_mount_EBUSY.h"


int test_yaffs_mount_EBUSY(void)
{
	int output=0;
	int error_code=0;

	output=yaffs_mount(YAFFS_MOUNT_POINT);
	if (output==-1){
		error_code=yaffs_get_error();
		//printf("EISDIR def %d, Error code %d\n", ENOTDIR,error_code);
		if (abs(error_code)==EBUSY){
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("mounted the same mount point twice.(which is a bad thing)\n", 2);
		return -1;
	}
}

int test_yaffs_mount_EBUSY_clean(void)
{
	return 1;
}

