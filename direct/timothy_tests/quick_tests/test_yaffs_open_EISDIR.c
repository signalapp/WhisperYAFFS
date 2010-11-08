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

#include "test_yaffs_open_EISDIR.h"

/*EISDIR is caused by trying to open a directory */

static int handle=0;
int test_yaffs_open_EISDIR(void){
	int output=0;
	int error_code=0;
	/*printf("path %s\n",path); */
	handle=yaffs_open(YAFFS_MOUNT_POINT, O_CREAT | O_TRUNC| O_RDWR ,FILE_MODE );
	if (handle==-1){
		error_code=yaffs_get_error();
		//printf("EISDIR def %d, Error code %d\n", EISDIR,error_code);
		if (abs(error_code)== EISDIR){
			return 1;
		}
		else {
			printf("different error than expected\n");
			return -1;
		}
	}
	else {
		printf("non existant directory opened.(which is a bad thing)\n");
		return -1;
	}

}
int test_yaffs_open_EISDIR_clean(void){
	if (handle >=0){
		return yaffs_close(handle);
	}
	else {
		return 1;	/* the file failed to open so there is no need to close it*/
	}
}

