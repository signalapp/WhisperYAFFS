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

#include "test_yaffs_rmdir_EBUSY.h"


int test_yaffs_rmdir_EBUSY(void)
{
	int output=0;
	int error_code =0;


	output = yaffs_rmdir("/yaffs2/");
	if (output<0){ 
		error_code=yaffs_get_error();
		if (abs(error_code)==EBUSY){
			return 1;
		} else {
			print_message("returned error does not match the the expected error\n",2);
			return -1;
		}
	} else{
		print_message("removed /yaffs2/ directory (which is a bad thing)\n",2);
		return -1;
	}	
}


int test_yaffs_rmdir_EBUSY_clean(void)
{
	int output = 0;

	return 1;

}

