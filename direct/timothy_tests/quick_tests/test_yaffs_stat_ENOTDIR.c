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

#include "test_yaffs_stat_ENOTDIR.h"

int test_yaffs_stat_ENOTDIR(void){
	int error_code=0;
	struct yaffs_stat stat;
	int output=0;
	char text[100];
	text[0] ='\0';
	output=yaffs_stat("/non-existing-dir/foo", &stat);;
	if (output<0){ 
		error_code=yaffs_get_error();
		if (abs(error_code)==ENOTDIR){
			return 1;
		}
		else {
			printf("returned error does not match the the expected error\n");
			return -1;
		}
	}
	else{
		printf("stated a non-existing file (which is a bad thing)\n");
		return -1;
	}	
}

int test_yaffs_stat_ENOTDIR_clean(void){
	return 1;
}
