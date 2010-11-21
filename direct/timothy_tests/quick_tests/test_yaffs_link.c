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

#include "test_yaffs_link.h"



int test_yaffs_link(void)
{
	int output=0;
	output = yaffs_link(FILE_PATH,HARD_LINK_PATH);
	return output;
}


int test_yaffs_link_clean(void)
{
	int output=0;	
	int error =0;
	output= yaffs_unlink(HARD_LINK_PATH);
	if (output<0){
		error=yaffs_get_error();
		if (abs(error)==ENOENT){
			//if the file does not exist then the error should be ENOENT.
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		return 1;	/* the file failed to open so there is no need to close it */
	}
}

