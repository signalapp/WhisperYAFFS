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

#include "test_yaffs_read_EBADF.h"

int test_yaffs_read_EBADF(void)
{
	int error_code=0;
	int output=0;
	char text[100];
	text[0] ='\0';

	output=yaffs_read(-1, text, FILE_TEXT_NBYTES);
	if (output<0){ 
		error_code = yaffs_get_error();
		if (abs(error_code) == EBADF){
			return 1;
		} else {
			print_message("returned error does not match the the expected error\n",2);
			return -1;
		}
	} else{
		print_message("read a non-existing file (which is a bad thing)\n",2);
		return -1;
	}	
}

int test_yaffs_read_EBADF_clean(void)
{
	return 1;
}
