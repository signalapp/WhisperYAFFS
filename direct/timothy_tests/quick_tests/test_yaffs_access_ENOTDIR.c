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

#include "test_yaffs_access_ENOTDIR.h"

static int handle=0;

int test_yaffs_access_ENOTDIR(void)
{
	int output=0;
	int error_code=0;
	
	output=yaffs_access("/nonexisting_dir/foo",0);
	if (output==-1){
		error_code=yaffs_get_error();
		if (abs(error_code)==ENOTDIR){
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("non existant directory accessed.(which is a bad thing)\n",2);
		return -1;
	}

}
int test_yaffs_access_ENOTDIR_clean(void)
{
	return 1;
}

