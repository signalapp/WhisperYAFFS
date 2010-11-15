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

#include "test_yaffs_fchmod.h"

static int handle = -1;

int test_yaffs_fchmod(void)
{
	int output = 0;
	
	handle = test_yaffs_open();

	if (handle < 0){
		print_message("failed to open file\n",2);
		return -1;
	}

	output = yaffs_fchmod(handle,S_IREAD|S_IWRITE);

	if (output>=0){
		/* there were no errors */
		return 1;
	} else {
		print_message("failed to chmod the file\n",2);
		return -1;
	}

}

int test_yaffs_fchmod_clean(void)
{
	if (handle >= 0) {
		return yaffs_close(handle);
	}
	return 1;

}
