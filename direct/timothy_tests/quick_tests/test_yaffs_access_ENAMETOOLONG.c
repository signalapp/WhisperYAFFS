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

#include "test_yaffs_access_ENAMETOOLONG.h"

int test_yaffs_access_ENAMETOOLONG(void)
{
	int output=0;
	int error=0;
	int file_name_length=1000000;
	char file_name[file_name_length];
	int x=0;
	for (x=0; x<file_name_length -1; x++){
		file_name[x]='a';
	}
	file_name[file_name_length-2]='\0';
	

	output= yaffs_access(file_name ,R_OK |W_OK);
	if (output<0){
		error=yaffs_get_error();
		if ( abs(error)== ENAMETOOLONG){
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

int test_yaffs_access_ENAMETOOLONG_clean(void)
{
	return EROFS_clean();
}
