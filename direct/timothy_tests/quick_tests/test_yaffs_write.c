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

#include "test_yaffs_write.h"

static int handle=0;
int test_yaffs_write(void){
	handle=test_yaffs_open();
	if (handle>=0){
		return yaffs_write(handle, FILE_TEXT, FILE_TEXT_NBYTES);
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_write_clean(void){
	if (handle>=0){
		return yaffs_close(handle);
	}
	else {
		return 1; /* no handle was opened so there is no need to close a handle */
	}	
}
