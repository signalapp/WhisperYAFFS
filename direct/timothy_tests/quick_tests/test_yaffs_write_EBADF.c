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

#include "test_yaffs_write_EBADF.h"

static int handle=0;
int test_yaffs_write_EBADF(void){
	int output=0;
	int error_code=0;
	handle=test_yaffs_open();
	if (handle>=0){
		output= yaffs_write(-1, FILE_TEXT, FILE_TEXT_NBYTES);
		if (output<0){
			error_code=yaffs_get_error();
			//printf("EISDIR def %d, Error code %d\n", ENOTDIR,error_code);
			if (abs(error_code)==EBADF){
				return 1;
			}
			else {
				printf("different error than expected\n");
				return -1;
			}
		}
		else {
			printf("wrote to a bad handle.(which is a bad thing)\n");
			return -1;
		}

	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_write_EBADF_clean(void){
	if (handle>=0){
		return yaffs_close(handle);
	}
	else {
		return 1; /* no handle was opened so there is no need to close a handle */
	}	
}
