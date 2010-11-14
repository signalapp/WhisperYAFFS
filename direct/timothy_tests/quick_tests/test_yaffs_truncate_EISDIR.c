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
static int handle= -1;

int test_yaffs_truncate_EISDIR(void)
{
	int error = 0;
	int output = 0;
	handle = test_yaffs_open();
	if (handle>=0){
		output = yaffs_truncate("/yaffs2/",10);
		if (output < 0){
			error = yaffs_get_error();
			if (abs(error) == EISDIR){
				return 1;
			} else {
				print_message("received a different error than expected\n",2);
				return -1;
			}
		} else{
			print_message("truncated a directory\n",2);
			return -1;
		}
			
	} else {
		print_message("error opening file\n",2);
		return 1;
	}
}

int test_yaffs_truncate_EISDIR_clean(void)
{
	return 1;
}
