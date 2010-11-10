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

#include "test_yaffs_write_EFBIG.h"

static int handle=0;
static char *file_name = NULL;
int test_yaffs_write_EFBIG(void)
{
	int output=0;
	int error_code=0;
	handle=test_yaffs_open();
	int x=0;
	
	long int file_name_length=100000000000;

	file_name = malloc(file_name_length);
	if(!file_name){
		printf("unable to create file text\n");
		return -1;
	}
	
	strcat(file_name,YAFFS_MOUNT_POINT);
	for (x=strlen(YAFFS_MOUNT_POINT); x<file_name_length -1; x++){
		file_name[x]='a';
	}
	file_name[file_name_length-2]='\0';



	if (handle>=0){
		output= yaffs_write(handle, file_name, file_name_length-1);
		if (output<0){
			error_code=yaffs_get_error();
			//printf("EISDIR def %d, Error code %d\n", ENOTDIR,error_code);
			if (abs(error_code)==EINVAL){
				return 1;
			} else {
				printf("different error than expected\n");
				return -1;
			}
		} else {
			printf("wrote a large amount of text to a file.(which is a bad thing)\n");
			return -1;
		}

	} else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_write_EFBIG_clean(void)
{
	int output=1;
	if(file_name){
		free(file_name);
		file_name = NULL;
	}

	if (handle>=0){	
		output=yaffs_close(handle);
	}

	
	output= test_yaffs_truncate_clean();	
	if (output>=0){
		output=test_yaffs_write();
		if (output>=0){
			return 1;
		} else {
			printf("failed to write to file\n");
			return -1;
		}
	} else {
		printf("failed to truncate file\n");
		return -1;
	}

}
