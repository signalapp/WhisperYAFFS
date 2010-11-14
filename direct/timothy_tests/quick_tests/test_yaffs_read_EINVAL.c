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

#include "test_yaffs_read_EINVAL.h"

static int handle = -1;
static char *file_name = NULL;

int test_yaffs_read_EINVAL(void)
{
	int error_code = 0;
	handle=test_yaffs_open();
	char text[2000000]="\0";
	int output=0;	
	
	if (handle<0){
		print_message("could not open file\n",2);
		return -1;
	}	

	/*there needs a large amout of test in the file in order to trigger EINVAL */
	output=test_yaffs_read_EINVAL_init();
	if (output<0){
		print_message("could not write text to the file\n",2);
		return -1; 
	}

	if (handle>=0){
		output=yaffs_read(handle, text, -1);
		if (output<0){ 
			error_code=yaffs_get_error();
			if (abs(error_code)== EINVAL){
				return 1;
			} else {
				print_message("different error than expected\n",2);
				return -1;
			}
		} else{
			print_message("read a negative number of bytes (which is a bad thing)\n",2);
			return -1;
		}
	} else {
		print_message("error opening file\n",2);
		return -1;
	}
}

int test_yaffs_read_EINVAL_clean(void)
{
	int output=0;
	if (handle>=0){
		output=test_yaffs_read_EINVAL_init_clean();
		if(output>=0){
			output=yaffs_close(handle);
			if (output>=0){
				return 1;
			} else {
				print_message("could not close the handle\n",2);
				return -1;
			}
		} else {
			print_message("failed to fix the file\n",2);
			return -1;
		}
	} else {
		print_message("no open handle\n",2);
	}
}

int test_yaffs_read_EINVAL_init(void)
{
	int output=0;
	int x=0;
	
	int file_name_length=1000000;

	file_name = malloc(file_name_length);
	if(!file_name){
		print_message("unable to create file text\n",2);
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
			print_message("could not write text to file\n",2);
			return -1;
		} else {
			
			return 1;
		}

	} else {
		print_message("error opening file\n",2);
		return -1;
	}
	
}

int test_yaffs_read_EINVAL_init_clean(void)
{
	int output=1;
	if(file_name){
		free(file_name);
		file_name = NULL;
	}

	
	output= yaffs_truncate(FILE_PATH,FILE_SIZE );	
	if (output>=0){
		output=test_yaffs_write();
		if (output>=0){
			return 1;
		} else {
			print_message("failed to write to file\n",2);
			return -1;
		}
	} else {
		print_message("failed to truncate file\n",2);
		return -1;
	}

}

