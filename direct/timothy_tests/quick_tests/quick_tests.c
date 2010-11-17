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

int main(int argc, char *argv[]){
	int output=0;
	char message[30];
	message[0]='\0';
	unsigned int x=0;

	init_quick_tests(argc, argv);
	print_message("\n\nrunning quick tests for yaffs\n\n", 0);

	for (x=0;x<total_number_of_tests;x++){
	/*	output=yaffs_open(FILE_PATH,O_CREAT | O_RDWR, FILE_MODE);
		printf("yaffs_open %d \n",output);
		printf("yaffs_close %d \n",yaffs_close(output));
	*/	yaffs_set_error(0);	/*reset the last error to 0 */
		sprintf(message,"\nrunning test: %s \n",test_list[x].name_of_test);
		print_message(message,3);
		output=test_list[x].p_function();	/*run test*/
		if (output>=0){
			/*test has passed*/
			sprintf(message,"\ttest %s passed\n",test_list[x].name_of_test);
			print_message(message,3); 
			num_of_tests_pass++;
		} else {
			/*test is assumed to have failed*/
			//printf("test failed\n");
			sprintf(message,"test: %s failed\n",test_list[x].name_of_test);
			print_message(message,1);		
			num_of_tests_failed ++;	
		
			get_error();
			print_message("\n\n",1);
			if (get_exit_on_error()){	
				quit_quick_tests(1);
			}

		}
		output=0;
		output=test_list[x].p_function_clean();	/*clean the test*/
		if (output <0){
			/* if the test failed to clean it's self then */
			sprintf(message,"test: %s failed to clean\n",test_list[x].name_of_test);
			print_message(message,1);		
			num_of_tests_failed ++;	
			num_of_tests_pass--;
			get_error();
			printf("\n\n");
			if (get_exit_on_error()){
				quit_quick_tests(1);
			}
			
		} else {
			sprintf(message,"\ttest clean: %s passed\n",test_list[x].name_of_test);
			print_message(message,3);
		}
	}
	/*this is where the loop should break to*/
	quit_quick_tests(0);
}

void quit_quick_tests(int exit_code)
{
	char message[30];
	message[0]='\0';	
	if (num_of_tests_pass==total_number_of_tests &&  num_of_tests_failed==0){
		printf("\t OK \n");
	}
	printf("out of %d tests, %d ran: %d passed and %d failed\n\n\n", total_number_of_tests,(num_of_tests_pass+num_of_tests_failed) ,num_of_tests_pass,num_of_tests_failed);
	yaffs_unmount(YAFFS_MOUNT_POINT);
	exit(exit_code);
}

void get_error(void)
{
	int error_code=0;
	char message[30];
	message[0]='\0';

	error_code=yaffs_get_error();
	sprintf(message,"yaffs_error code %d\n",error_code);
	print_message(message,1);
	sprintf(message,"error is : %s\n",yaffs_error_to_str(error_code));
	print_message(message,1);
}

void init_quick_tests(int argc, char *argv[])
{
	int trace=0;
	int x=0;	
	for (x = 0; x < argc; x++){
		if (0==strcmp(argv[x],"-h")){
			printf("help\n");
			printf("-h will print the commands available\n");
			printf("-c will continue after a test failes else the program will exit\n");
			printf("-v will print all messages\n");
			printf("-q quiet mode only the number of tests passed and failed will be printed\n");
			printf("-t [number] set yaffs_trace to number\n");
			exit(0);
		} else if (0==strcmp(argv[x],"-c")) {
			set_exit_on_error(0);
		} else if (0==strcmp(argv[x],"-q")) {
			set_print_level(-3);
		} else if (0==strcmp(argv[x],"-t")) {
			trace = atoi(argv[x+1]);
		}  else if (0==strcmp(argv[x],"-v")) {
			set_print_level(5);
		}

	}
	yaffs_start_up();
	yaffs_set_trace(trace);

}
