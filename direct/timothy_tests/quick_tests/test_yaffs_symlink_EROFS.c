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

#include "test_yaffs_symlink_EROFS.h"

static int output = 0;

int test_yaffs_symlink_EROFS(void)
{
	int error_code = 0;

	EROFS_setup();
	output = yaffs_symlink(FILE_PATH,SYMLINK_PATH);
	if (output<0){ 
		error_code=yaffs_get_error();
		if (abs(error_code)==EROFS){
			return 1;
		} else {
			print_message("returned error does not match the the expected error\n",2);
			return -1;
		}
	} else {
		print_message("created a symlink over an existing symlink (which is a bad thing)\n",2);
		return -1;
	}	

}

int test_yaffs_symlink_EROFS_clean(void)
{
	return (EROFS_clean() && yaffs_unlink(SYMLINK_PATH));
}



