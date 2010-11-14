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

/* yaffs will open a file without an error with the creation mode set to 255.*/

#include "test_yaffs_open_EINVAL2.h"

static int handle = -1;

int test_yaffs_open_EINVAL2(void)
{

	handle=yaffs_open(FILE_PATH, O_CREAT | O_RDWR ,255);

	/* yaffs_open does not check the modes passed into it. This means that the file should open */
	if (handle < 0){
		print_message("file not opened with bad creation mode set (which is a bad thing)\n",2);
		return -1;
	} else {
		/* file opened */
		return 1;
	}
}

int test_yaffs_open_EINVAL2_clean(void)
{
	if (handle >= 0){
		return yaffs_close(handle);
	} else {
		return 1;	/* the file failed to open so there is no need to close it*/
	}
}

