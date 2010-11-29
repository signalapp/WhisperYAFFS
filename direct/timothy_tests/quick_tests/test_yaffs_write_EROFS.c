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

#include "test_yaffs_write_EROFS.h"

static int handle= -1;

int test_yaffs_write_EROFS(void)
{
	int output=0;
	int error_code=0;
	EROFS_setup();
	handle=yaffs_open(FILE_PATH, O_CREAT  ,S_IREAD);
	if (handle>=0){
		output= yaffs_write(handle, FILE_TEXT, FILE_TEXT_NBYTES);
		if (output<0){
			error_code=yaffs_get_error();
			//printf("EISDIR def %d, Error code %d\n", ENOTDIR,error_code);
			if (abs(error_code)==EROFS){
				return 1;
			} else {
				print_message("different error than expected\n",2);
				return -1;
			}
		} else {
			print_message("wrote to a bad handle.(which is a bad thing)\n",2);
			return -1;
		}

	} else {
		print_message("error opening file\n",2);
		return -1;
	}
}

int test_yaffs_write_EROFS_clean(void)
{
	int output=1;
	if (handle>=0){
		output= yaffs_close(handle);
	} 
	return (EROFS_clean() && output);
}
