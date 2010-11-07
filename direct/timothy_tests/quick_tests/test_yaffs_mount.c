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

#include "test_yaffs_mount.h"

int test_yaffs_mount(void){
	int output=0;
	output=yaffs_mount(YAFFS_MOUNT_POINT);
	/*printf("output %d",output);*/
	return output;
}

int test_yaffs_mount_clean(void){
	return 1;
	
}
