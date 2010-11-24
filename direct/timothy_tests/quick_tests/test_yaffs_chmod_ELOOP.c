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

#include "test_yaffs_chmod_ELOOP.h"

int test_yaffs_chmod_ELOOP(void)
{
	int error=0;
	int output;

	if (set_up_ELOOP()<0){
		print_message("failed to setup symlinks\n",2);
		return -1;
	}

	output = yaffs_chmod(ELOOP_PATH,S_IREAD|S_IWRITE);

	if (output<0){
		error=yaffs_get_error();
		if (abs(error)==ELOOP){
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("chmoded the ELOOP (which is a bad thing)\n",2);
		return -1;
	}

}

int test_yaffs_chmod_ELOOP_clean(void)
{
	return test_yaffs_chmod();
}
