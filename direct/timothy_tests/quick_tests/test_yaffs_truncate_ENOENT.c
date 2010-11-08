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

#include "test_yaffs_truncate_ENOENT.h"
static int handle=0;

int test_yaffs_truncate_ENOENT(void){
	int error=0;
	int output=0;
	handle=test_yaffs_open();
	if (handle>=0){
		output= yaffs_truncate("/yaffs2/non_existing_file",FILE_SIZE_TRUNCATED );
		if (output<0){
			error=yaffs_get_error();
			if (abs(error)==ENOENT){
				return 1;
			}
			else {
				printf("recieved a different error than expected\n");
				return -1;
			}
		}
		else{
			printf("truncated a nonexisting file\n");
			return -1;
		}
			
	}
	else {
		printf("error opening file");
		return -1;
	}
}

int test_yaffs_truncate_ENOENT_clean(void){
	return 1;
}
