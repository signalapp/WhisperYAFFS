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

#include "test_yaffs_truncate_EFBIG.h"
static int handle=0;

int test_yaffs_truncate_EFBIG(void){
	int error=0;
	int output=0;
	handle=test_yaffs_open();
	if (handle>=0){
		output= yaffs_truncate("/yaffs2/foo",10000000000000000000000000000000000000000000 );
		if (output<0){
			error=yaffs_get_error();
			if (abs(error)==EINVAL){
				return 1;
			}
			else {
				printf("recieved a different error than expected\n");
				return -1;
			}
		}
		else{
			printf("truncated a file to a massive size\n");
			return -1;
		}
			
	}
	else {
		printf("error opening file");
		return -1;
	}
}

int test_yaffs_truncate_EFBIG_clean(void){
	return 1;
}
