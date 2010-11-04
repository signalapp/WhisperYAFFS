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

#include "quick_tests.h"


int random_seed;
int simulate_power_failure = 0;





static unsigned int num_of_tests_pass=0;
static unsigned int num_of_tests_failed=0;
static unsigned int total_number_of_tests=(sizeof(test_list)/sizeof(test_template));

int main(){
	int output=0;


	unsigned int x=0;	
	init_quick_tests();
	printf("\n\nrunning quick tests for yaffs\n");
	//printf("testing yaffs\n");

	//printf("len function %d",(sizeof(test_list)/sizeof(test_template)));
	for (x=0;x<total_number_of_tests;x++){
		//printf("x %d\n",x);
		yaffs_set_error(0);	/*reset the last error to 0 */
		output=test_list[x].p_function();	/*run test*/
		if (output>=0){
			/*test has passed*/
			num_of_tests_pass++;
		}
		else {
			/*test is assumed to have failed*/
			//printf("test failed\n");
			printf("test: %s failed\n",test_list[x].fail_message);		
			num_of_tests_failed ++;	
			quit_quick_tests(1);
			
		}
		output=test_list[x].p_function_clean();	/*clean the test*/
		if (output <0){
			/* if the test failed to clean it's self then */
			printf("test: %s failed to clean\n",test_list[x].fail_message);		
			num_of_tests_failed ++;	
			num_of_tests_pass--;
			quit_quick_tests(1);
		}
			
	}
	/*this is where the loop should break to*/
	quit_quick_tests(0);
	/* the progame should never get here*/	
	return 0;
}

void quit_quick_tests(int exit_code){
	int error_code=0;
	if (num_of_tests_pass==total_number_of_tests &&  num_of_tests_failed==0){
		printf("\t OK \n");
	}
	else {
		error_code=yaffs_get_error();
		printf("yaffs_error code %d\n",error_code);
		printf("error is : %s\n",yaffs_error_to_str(error_code));
	}
	printf("tests: %d passed %d failed\n\n\n",num_of_tests_pass,num_of_tests_failed);
	yaffs_unmount(YAFFS_MOUNT_POINT);
	exit(exit_code);
}

void init_quick_tests(void){
	yaffs_start_up();;
	yaffs_set_trace(0);

}
