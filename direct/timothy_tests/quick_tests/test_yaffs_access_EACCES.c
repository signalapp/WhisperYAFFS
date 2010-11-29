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

#include "test_yaffs_access_EACCES.h"

int test_yaffs_access_EACCES(void)
{
	int output=0;
	int error=0;
	output=yaffs_chmod(FILE_PATH,S_IREAD);
	if (output<0){
		print_message("failed to chmod file\n",2);
		return -1;
	}
	output= yaffs_access(FILE_PATH,W_OK);
	if (output<0){
		error=yaffs_get_error();
		if ( abs(error)== EACCES){
			return 1;
		} else {
			print_message("error does not match expected error\n",2);
			return -1;
		}
	} else{
		print_message("accessed an existing file with bad mode (which is a bad thing\n",2);

		return -1;
	}
}

int test_yaffs_access_EACCES_clean(void)
{
	int output=0;
	output=yaffs_chmod(FILE_PATH,S_IREAD|S_IWRITE);
	if (output<0){
		print_message("failed to chmod file\n",2);
		return -1;
	}
	return 1;
}
