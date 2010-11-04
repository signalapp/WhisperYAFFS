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

#include "test_yaffs_read.h"

static int handle=0;

int test_yaffs_read(void){
	handle=test_yaffs_open();
	char text[20]="\0";
	int output=0;
	//printf("handle %d\n",handle);
	if (handle>=0){
		output=yaffs_read(handle, text, FILE_TEXT_NBYTES);
		//printf("yaffs_test_read output: %d\n",output);
		//printf("text in file is: '%s' expected text is '%s'\n",text,FILE_TEXT);
		if (output>0){ 
			if (0==memcmp(text,FILE_TEXT,FILE_TEXT_NBYTES)){
				return 1;
			}
			else {
				printf("returned text does not match the the expected text that should be in the file\n");
				return -1;
			}
		}
		else{
			printf("error reading file\n");
			return -1;
		}
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_read_clean(void){
	return yaffs_close(handle);
}
