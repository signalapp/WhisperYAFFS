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

#include "test_yaffs_truncate.h"
static int handle=0;

int test_yaffs_truncate(void){
	handle=test_yaffs_open();
	if (handle>0){
		return yaffs_truncate(FILE_PATH,FILE_SIZE_TRUNCATED );
	}
	else {
		printf("error opening file");
		return -1;
	}
}

int test_yaffs_truncate_clean(void){
	/* change file size back to orignal size */
	int handle=test_yaffs_open();
	int output=0;
	if (handle>0){
		output= yaffs_truncate(FILE_PATH,FILE_SIZE );
		if (output>=0){
			return yaffs_close(handle);
		}
	}
	else {
		printf("error opening file in clean function");
		return -1;
	}

}
