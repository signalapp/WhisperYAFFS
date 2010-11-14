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

#include "test_yaffs_open_ENOENT.h"

static int handle = -1;

int test_yaffs_open_ENOENT(void)
{
	int output = 0;
	int error_code = 0;
	/*printf("path %s\n",path); */
	handle = yaffs_open("/yaffs2/non_existant_file", O_TRUNC| O_RDWR,FILE_MODE );
	if (handle == -1){
		error_code = yaffs_get_error();
		if (abs(error_code) == ENOENT){
			return 1;
		}
		else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("non existant file opened.(which is a bad thing)\n",2);
		return -1;
	}
}

int test_yaffs_open_ENOENT_clean(void)
{
	if (handle >=0){
		return yaffs_close(handle);
	}
	else {
		return 1;	/* the file failed to open so there is no need to close it*/
	}
}

