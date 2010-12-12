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

#include "handle_test.h"

int random_seed;
int simulate_power_failure = 0;

int main()
{
	
	int test_batch = 1000000;
	int output =0;
	unsigned int total_number_of_tests_run=0;
	yaffs_start_up();
	yaffs_mount("yaffs2");

	printf("running tests\n");

	output=dup_test();
	printf("dup test: %d\n ",output);

	while (1){
		output = open_close_handle_test(test_batch);
		if (output>=0){
			total_number_of_tests_run ++;
		} else {
			get_error();
			return -1;
		}
		printf("total number of tests = %d\n",(total_number_of_tests_run*test_batch));
	}


	return 0;
}
int dup_test(void){
	int handle = -1;
	int handle2 = -1;
	int output =-1;

	handle =open_handle();
	if (handle<0){
		printf("error: failed to open handle\n");
		return -1;
	}
	handle2=yaffs_dup(handle);
	if (handle2<0){
		printf("error: failed to open handle2\n");
		return -1;
	}
	
	output=yaffs_lseek(handle,20,SEEK_SET);
	if (output >= 0) {
		output = yaffs_lseek(handle,0,SEEK_CUR);
		if (output = 20){
			printf("dup is working\n");
			return 1;
		} else if (output <0){
			printf("failed to lseek the second time\n");
			return -1;
		} else {
			printf("lseek position is diffrent than expected\n");
			return -1;
		}
	} else {
		printf("failed to lseek the first time\n");
		return -1;
	}

}

int open_close_handle_test(int num_of_tests)
{
	int handle_number=open_handle();
	int handle=0;
	int x =0;
	int output=0;
	yaffs_close(handle_number);
	for (x=0;x<num_of_tests;x++){
		handle=open_handle();
		if (handle != handle_number){
			printf("handle number changed\n");
			printf("test number= %d\n",x);
			printf("expected number = %d\n",handle_number);
			printf("handle_number = %d\n",handle);
			return -1;
		}
		output=yaffs_close(handle);
		if (output <0){
			printf("failed to close file\n");
			return -1;
		}
	}
	return 1;
}

void get_error(void)
{
	int error_code=0;
	error_code=yaffs_get_error();
	printf("yaffs_error code %d\n",error_code);
	printf("error is : %s\n",yaffs_error_to_str(error_code));
}

int open_handle(void)
{
	return yaffs_open(FILE_PATH,O_CREAT | O_RDWR,S_IREAD | S_IWRITE);
}

