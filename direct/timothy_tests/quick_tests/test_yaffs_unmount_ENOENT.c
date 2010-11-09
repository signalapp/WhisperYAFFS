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

#include "test_yaffs_mount_ENOENT.h"


int test_yaffs_unmount_ENOENT(void){
	int output=0;
	int error_code=0;
	/*printf("path %s\n",path); */
	
	

	output=yaffs_unmount("/non_existaint_mount_point/");
	if (output==-1){
		error_code=yaffs_get_error();
		if (abs(error_code)==EINVAL){
			return 1;
		}
		else {
			printf("different error than expected\n");
			return -1;
		}
	}
	else {
		printf("non existant mount point unmounted.(which is a bad thing)\n");
		return -1;
	}

}
int test_yaffs_unmount_ENOENT_clean(void){
	return 1;
}

