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

#include "test_yaffs_fstat.h"

static int handle=0;
int test_yaffs_fstat(void){
	struct yaffs_stat stat;
	handle=test_yaffs_open();
	if (handle>=0){
		return 	yaffs_fstat(handle , &stat);;
	}
	else {
		printf("error opening file\n");
		return -1;
	}
}

int test_yaffs_fstat_clean(void){
	if (handle<0){
		return yaffs_close(handle);
	}
	return 1;
}
