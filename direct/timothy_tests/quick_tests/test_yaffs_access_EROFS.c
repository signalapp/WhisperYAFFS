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

#include "test_yaffs_access_EROFS.h"

int test_yaffs_access_EROFS(void)
{
	int output=0;
	int error=0;
	if (EROFS_setup() < 0 ){
		return -1;
	}

	output= yaffs_access(FILE_PATH,R_OK |W_OK);
	if (output<0){
		error=yaffs_get_error();
		if ( abs(error)== EROFS){
			return 1;
		} else {
			print_message("error does not match expected error\n",2);
			return -1;
		}
	} else{
		print_message("accessed an existing file with the file system mounted as read only (which is a bad thing)\n",2);

		return -1;
	}
}

int test_yaffs_access_EROFS_clean(void)
{
	return 1;
	return EROFS_clean();
}
