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

/* generates a EBADF error by closing a handle twice */

#include "test_yaffs_open.h"

static int handle=0;

int test_yaffs_close_EBADF(void){
	int output=0;
	int error_code=0;
	handle=test_yaffs_open();
	if (handle>=0){
		output=yaffs_close(handle);
		if (output>=0){
			output=yaffs_close(handle);
			if (output<0){
				/*error has happened */
				error_code=yaffs_get_error();
				printf("EEXIST def %d, Error code %d\n",(- EBADF),error_code);
				if (abs(error_code)== EBADF){
					return 1;
				}
				else {
					printf("different error than expected\n");
					return -1;
				}
			}
			else {
				printf("closed the same handle twice. (which is a bad thing)\n");
				return -1;
			}
		}
		else {
			printf("failed to close the handle the firs time\n");
			return -1;
		}	
	}
	else {
		printf("failed to open file\n");
		return -1;
	}

}


int test_yaffs_close_EBADF_clean(void){
	/* there is nothing that needs to be recreated */	
	return 1;
}

