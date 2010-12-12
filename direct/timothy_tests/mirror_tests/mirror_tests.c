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

#include "mirror_tests.h"

int random_seed;
int simulate_power_failure = 0;





typedef struct test_list_temp2{
	char *test_name;
	int (*test_pointer)(arg_temp *args_struct);
}test_list_temp;

typedef struct test_temp2 {
	int num_of_tests;
	test_list_temp test_list[];
}test_temp;

test_temp yaffs_tests={
	0,
	{{"yaffs_test_open",yaffs_test_open}
	}
};

test_temp linux_tests={
	0,
	{{"linux_test_open",linux_test_open}
	}
};


int main(int argc, char *argv[])
{
	char message[100];

	yaffs_tests.num_of_tests=(sizeof(yaffs_tests)/sizeof(test_temp));
	linux_tests.num_of_tests=(sizeof(linux_tests)/sizeof(test_temp));

	init(argc,argv);
	print_message(1,"running mirror_tests\n");
	sprintf(message,"linux_root_path: %s\n",linux_struct.root_path);
	print_message(3,message);
	sprintf(message,"yaffs_root_path: %s\n",yaffs_struct.root_path);
	print_message(3,message);
	sprintf(message,"linux_num_of_tests: %d\n",linux_tests.num_of_tests);
	print_message(3,message);
	sprintf(message,"yaffs_num_of_tests: %d\n",yaffs_tests.num_of_tests);
	print_message(3,message);

		run_random_test();
	return 0;
}

void init(int argc, char *argv[])
{
	char dir[200];
	dir[0]='\0';
	int x=-1;
	char message[100];
	linux_struct.type_of_test =LINUX;
	yaffs_struct.type_of_test =YAFFS;

	sprintf(message,"current absolute path is: %s\n",getcwd(dir,200));
	print_message(3,message);
	strcpy(dir,getcwd(dir,200));

	strcat(dir,"/test");
	printf("dir: %s\n",dir);
	strcpy(linux_struct.root_path,dir);
	strcpy(yaffs_struct.root_path,"yaffs2/test/");	


	for (x=0;x<argc;x++){
		if (strcmp(argv[x],"-h")==0){
			printf("mirror_tests help\n");
			printf("arguments:\n");
			printf("\t-yaffs_path [PATH] //sets the path for yaffs.\n");
			printf("\t-linux_path [PATH] //sets the path for linux.\n");
			printf("\t-p [NUMBER] //sets the print level for mirror_tests.\n");
			printf("\t-v //verbose mode everything is printed\n");
			printf("\t-q //quiet mode nothing is printed.\n");
			exit(0);
		} else if (strcmp(argv[x],"-yaffs_path")==0){
			strcpy(yaffs_struct.root_path, argv[x+1]);
		} else if (strcmp(argv[x],"-linux_path")==0){
			strcpy(linux_struct.root_path, argv[x+1]);
		} else if (strcmp(argv[x],"-p")==0){
			set_print_level(atoi(argv[x+1]));
		} else if (strcmp(argv[x],"-v")==0){
			set_print_level(5);
		} else if (strcmp(argv[x],"-q")==0){
			set_print_level(-1);
		} 
	}

	yaffs_start_up();
	print_message(message,"\nmounting yaffs\n");
	x=yaffs_mount("yaffs2");
	if (x<0) {
		print_message(3,"failed to mount yaffs\n");
		get_error_yaffs();
	} else {
		print_message(3,"mounted yaffs\n");
	}


	print_message(3,"\nmaking linux test dir\n");
	x=mkdir(linux_struct.root_path,0777);
	if (x<0) {
		print_message(1,"failed to make dir\n");
		get_error_linux();
	} else {
		print_message(3,"made dir\n");
	}

	print_message(3,"\nmaking yaffs test dir\n");
	x=yaffs_mkdir(yaffs_struct.root_path,0777);
	if (x<0) {
		print_message(1,"failed to make dir\n");
		get_error_yaffs();
	} else {
		print_message(3,"made dir\n");
	}
}

int run_random_test(void)
{
	int x=-1;
	int id=0;
	int test_id=-1;
	int num_of_tests=1;
	char message[15];
	arg_temp args_struct;
	for (x=0;x<num_of_tests;x++) {
		test_id = select_test_id(yaffs_tests.num_of_tests);
		sprintf(message,"test_id %d\n",test_id);
		print_message(2,message);
		generate_random_numbers(&args_struct);
		run_yaffs_test(id, &args_struct);
		run_linux_test(id, &args_struct);
	}
	compare_linux_and_yaffs(); 
}

int select_test_id(int test_len)
{
	int id=0;
	id=(rand() % test_len );
	return id;

}

int compare_linux_and_yaffs(void)
{
//	generate_array_of_objects_in_yaffs(); 
//	generate_array_of_objects_in_linux();
	//first do a check to see if both sides have the same objects on both sides. 
	//then stat all of the files and compare size and mode
	//read the text of each file and compare them.
	
	//show the diffrences by printing them. 

}

void generate_random_numbers(arg_temp *args_struct)
{
	args_struct->char1= (rand() % 255);
	args_struct->char2= (rand() % 255);
	args_struct->int1= (rand() % 100000);
	args_struct->int2= (rand() % 100000);
	strcpy(args_struct->string1, "apple");
	strcpy(args_struct->string2, "apple");
}

void run_yaffs_test(int id,arg_temp *args_struct)
{
	char message[30];
	int output =0;
	print_message(2,"\n");
	sprintf(message,"running_test %s\n",yaffs_tests.test_list[id].test_name);
	print_message(3,message);
	output=yaffs_tests.test_list[id].test_pointer(args_struct);
	if (output<0) {
		sprintf(message,"test_failed %s\n",yaffs_tests.test_list[id].test_name);
		print_message(1,message);
		get_error_yaffs();
	} else {
		print_message(3,"test_passed\n");
	}
}

void run_linux_test(int id,arg_temp *args_struct)
{
	char message[30];
	int output =0;
	print_message(2,"\n");
	sprintf(message,"running_test %s\n",linux_tests.test_list[id].test_name);
	print_message(3,message);
	output=linux_tests.test_list[id].test_pointer(args_struct);
	if (output<0) {
		sprintf(message,"test_failed %s\n",linux_tests.test_list[id].test_name);
		print_message(1,message);
		get_error_linux();
	} else {
		print_message(3,"test_passed\n");
	}
}

void get_error_yaffs(void)
{
	int error_code=0;
	char message[30];
	message[0]='\0';

	error_code=yaffs_get_error();
	sprintf(message,"yaffs_error code %d\n",error_code);
	print_message(1,message);
	sprintf(message,"error is : %s\n",yaffs_error_to_str(error_code));
	print_message(1,message);
}

void get_error_linux(void)
{
	int error_code=0;
	char message[30];
	message[0]='\0';

	error_code=errno;
	sprintf(message,"linux_error code %d\n",error_code);
	print_message(1,message);
	strcpy(message,"error code");
	sprintf(message,"error is : %s\n",yaffs_error_to_str(error_code));
	//perror(message);	
	print_message(1,message);
}

