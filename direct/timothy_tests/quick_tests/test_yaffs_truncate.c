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
static int handle = -1;

int test_yaffs_truncate(void)
{
	handle = test_yaffs_open();
	if (handle >= 0){
		return yaffs_truncate(FILE_PATH,FILE_SIZE_TRUNCATED );
	} else {
		print_message("error opening file",2);
		return -1;
	}
}

int test_yaffs_truncate_clean(void)
{
	/* change file size back to orignal size */
	int output=0;
	if (handle >= 0){
		output= yaffs_truncate(FILE_PATH,FILE_SIZE );
		if (output>=0){
			return yaffs_close(handle);
		} else {
			print_message("failed to truncate file\n",2);
			return -1;
		}
	} else {
		print_message("error opening file in clean function\n",2);
		return -1;
	}

}
