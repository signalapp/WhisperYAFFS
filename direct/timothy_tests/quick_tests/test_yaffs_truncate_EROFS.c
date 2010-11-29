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

#include "test_yaffs_truncate_EROFS.h"



int test_yaffs_truncate_EROFS(void)
{
	int error=0;
	int output=0;


	EROFS_setup();
	output= yaffs_truncate(FILE_PATH,FILE_SIZE_TRUNCATED );
	if (output<0){
		error=yaffs_get_error();
		if (abs(error)==EROFS){
			return 1;
		} else {
			print_message("received a different error than expected\n",2);
			return -1;
		}
	} else{
		print_message("truncated a file with EROFS\n",2);
		return -1;
	}
}

int test_yaffs_truncate_EROFS_clean(void)
{
	return 	EROFS_clean();
}
