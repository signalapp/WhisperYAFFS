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

#include "test_yaffs_unlink_ENAMETOOLONG.h"



int test_yaffs_unlink_ENAMETOOLONG(void)
{
	int output=0;
	int error_code=0;
	int x=0;
	int file_name_length=1000000;
	char file_name[file_name_length];
	
	strcat(file_name,YAFFS_MOUNT_POINT);
	for (x=strlen(YAFFS_MOUNT_POINT); x<file_name_length -1; x++){
		file_name[x]='a';
	}
	file_name[file_name_length-2]='\0';

	output=yaffs_unlink(file_name);
	if (output==-1){
		error_code=yaffs_get_error();
		if (abs(error_code)== EISDIR){
			return 1;
		} else {
			print_message("different error than expected\n",2);
			return -1;
		}
	} else {
		print_message("directory unlinked opened.(which is a bad thing)\n",2);
		return -1;
	}
}

int test_yaffs_unlink_ENAMETOOLONG_clean(void)
{
	return 1;
}

