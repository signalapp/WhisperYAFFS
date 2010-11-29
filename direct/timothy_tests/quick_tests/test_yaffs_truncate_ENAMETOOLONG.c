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

#include "test_yaffs_truncate_ENAMETOOLONG.h"



int test_yaffs_truncate_ENAMETOOLONG(void)
{
	int error=0;
	int output=0;

	int file_name_length=1000000;
	char file_name[file_name_length];
	int x=0;
	for (x=0; x<file_name_length -1; x++){
		file_name[x]='a';
	}
	file_name[file_name_length-2]='\0';

	output= yaffs_truncate(file_name,FILE_SIZE_TRUNCATED );
	if (output<0){
		error=yaffs_get_error();
		if (abs(error)==ENAMETOOLONG){
			return 1;
		} else {
			print_message("received a different error than expected\n",2);
			return -1;
		}
	} else{
		print_message("truncated a nonexisting file\n",2);
		return -1;
	}
}

int test_yaffs_truncate_ENAMETOOLONG_clean(void)
{
	return 1;
}
