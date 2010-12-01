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

#include "test_yaffs_dup.h"

static int handle = -1;
static int handle2 = -1;
int test_yaffs_dup(void)
{
	handle = yaffs_open(FILE_PATH,O_CREAT | O_RDWR, FILE_MODE);
	if (handle >=0){
		handle2 =yaffs_dup(handle);
		return handle2;
	}
	
}


int test_yaffs_dup_clean(void)
{
	int output1=1;
	int output2=1;
	if (handle >= 0){
		output1= yaffs_close(handle);
	} 
	if (handle2 >= 0){
		output2 =yaffs_close(handle2);
	}
	return (output1 && output2);	
}
