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

int num_of_random_tests=1;



typedef struct test_list_temp2{
	char *test_name;
	int (*test_pointer)(arg_temp *args_struct);
}test_list_temp;

typedef struct test_temp2 {
	int num_of_tests;
	test_list_temp test_list[];
}test_temp;

test_temp yaffs_tests={
	3,
	{{"yaffs_test_open",yaffs_test_open},
	{"yaffs_test_truncate",yaffs_test_truncate},
	{"yaffs_test_unlink",yaffs_test_unlink}
	}
};

test_temp linux_tests={
	3,
	{{"linux_test_open",linux_test_open},
	{"linux_test_truncate",linux_test_truncate},
	{"linux_test_unlink",linux_test_unlink}
	}
};


int main(int argc, char *argv[])
{
	char message[100];
	int x;
//	yaffs_tests.num_of_tests=(sizeof(yaffs_tests)/sizeof(test_temp));
//	linux_tests.num_of_tests=(sizeof(linux_tests)/sizeof(test_temp));

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

	run_random_test(num_of_random_tests);
	compare_linux_and_yaffs();
	yaffs_unmount("yaffs2");
	return 0;
}

void init(int argc, char *argv[])
{
	char dir[200];
	dir[0]='\0';
	int x=-1;
	char message[100];

	srand((unsigned)time(NULL));
	yaffs_set_trace(0);
	linux_struct.type_of_test =LINUX;
	yaffs_struct.type_of_test =YAFFS;

	sprintf(message,"current absolute path is: %s\n",getcwd(dir,200));
	print_message(3,message);
	strcpy(dir,getcwd(dir,200));

	strcat(dir,"/test/");
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
			printf("\t-n [number] //sets the number of random tests to run.\n");
			printf("\t-s [number] //seeds rand with the number\n");
			printf("\t-t [number] //sets yaffs_trace to the number\n");
			printf("\t-clean //removes emfile and test dir\n");
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
		} else if (strcmp(argv[x],"-n")==0){
			num_of_random_tests=atoi(argv[x+1]);
		} else if (strcmp(argv[x],"-s")==0){
			srand(atoi(argv[x+1]));
		} else if (strcmp(argv[x],"-t")==0){
			yaffs_set_trace(atoi(argv[x+1]));
		} else if (strcmp(argv[x],"-clean")==0){
			clean_dir();
			exit(0);
		}
	}
	clean_dir();
	yaffs_start_up();
	print_message(2,"\nmounting yaffs\n");
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

int run_random_test(int num_of_random_tests)
{
	int y=0;
	int x=-1;
	int id=0;
	int test_id=-1;
	int num_of_tests_before_check=10;
	char message[100];
	arg_temp args_struct;
	for (y=0;(y*num_of_tests_before_check)<num_of_random_tests;y++){
		for (x=0;x<num_of_tests_before_check;x++) {
			errno=0;
			yaffs_set_error(0);
			test_id = select_test_id(yaffs_tests.num_of_tests);
			sprintf(message,"running test_id %d\n",test_id);
			print_message(3,message);
			generate_random_numbers(&args_struct);
			run_yaffs_test(test_id, &args_struct);
			run_linux_test(test_id, &args_struct);
			if 	((abs(yaffs_get_error())!=abs(errno)) &&
				(abs(yaffs_get_error())!=EISDIR && abs(errno) != 0) &&
				(abs(yaffs_get_error())!=ENOENT && abs(errno) != EACCES)
				){
				print_message(2,"\ndifference in returned errors######################################\n");
				get_error_yaffs();
				get_error_linux();
				if (get_exit_on_error()){ 
					exit(0);
				}
			}
		}	
		compare_linux_and_yaffs();
	} 
}

int select_test_id(int test_len)
{
	int id=0;
	//printf("test_len = %d\n",test_len);
	id=(rand() % test_len );
	//printf("id %d\n",id);
	return id;

}

int compare_linux_and_yaffs(void)
{
	int x=0;
	yaffs_DIR *yaffs_open_dir;
	yaffs_dirent *yaffs_current_file;
	
	DIR *linux_open_dir;
	struct dirent *linux_current_file;

	yaffs_open_dir = yaffs_opendir(yaffs_struct.root_path);
	if (yaffs_open_dir) {
		for (x=0;NULL!=yaffs_readdir(yaffs_open_dir);x++){}
		printf("number of files in yaffs dir= %d\n",x);
		
		char yaffs_file_list[x][100];
		yaffs_rewinddir(yaffs_open_dir);
	
		for (x=0 ;NULL!=yaffs_current_file;x++)
		{
			yaffs_current_file =yaffs_readdir(yaffs_open_dir);
			if (NULL!=yaffs_current_file){
				strcpy(yaffs_file_list[x],yaffs_current_file->d_name);
			}
		}
	}

	linux_open_dir = opendir(linux_struct.root_path);
	if (linux_open_dir){
		for (x=0;NULL!=readdir(linux_open_dir);x++){}
		printf("number of files in linux dir= %d\n",(x-2));	
		//the -2 is because linux shows 2 extra files which are automaticly created. 
	
		char linux_file_list[x][100];
	
		for (x=0 ;NULL!=linux_current_file;x++)
		{
			linux_current_file =readdir(linux_open_dir);
			if (NULL!=linux_current_file){
				strcpy(linux_file_list[x],linux_current_file->d_name);
			}
		}
	}




	
	//printf("file_name %s\n", yaffs_current_file->d_name);
//	generate_array_of_objects_in_yaffs(); 
//	generate_array_of_objects_in_linux();
	//first do a check to see if both sides have the same objects on both sides. 
	//then stat all of the files and compare size and mode
	//read the text of each file and compare them.
	
	//show the diffrences by printing them. 

}

void generate_random_numbers(arg_temp *args_struct)
{
	char string[51];
	args_struct->char1= (rand() % 255);
	args_struct->char2= (rand() % 255);
	args_struct->int1= (rand() % 100000);
	args_struct->int2= (rand() % 100000);
	generate_random_string(string,50);
	strcpy(args_struct->string1, string);
	generate_random_string(string,50);
	strcpy(args_struct->string2, string);
}

void run_yaffs_test(int id,arg_temp *args_struct)
{
	char message[200];
	int output =0;
	print_message(3,"\n");
	//printf("id = %d\n",id);
	sprintf(message,"running_test %s\n",yaffs_tests.test_list[id].test_name);
	print_message(3,message);
	output=yaffs_tests.test_list[id].test_pointer(args_struct);
	if (output<0) {
		sprintf(message,"test_failed %s\n",yaffs_tests.test_list[id].test_name);
		print_message(3,message);
	} else {
		print_message(3,"test_passed\n");
	}
}

void run_linux_test(int id,arg_temp *args_struct)
{
	char message[200];
	int output =0;
	print_message(3,"\n");
	//printf("id = %d\n",id);
	sprintf(message,"running_test %s\n",linux_tests.test_list[id].test_name);
	print_message(3,message);
	output=linux_tests.test_list[id].test_pointer(args_struct);
	if (output<0) {
		sprintf(message,"test_failed %s\n",linux_tests.test_list[id].test_name);
		print_message(3,message);
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

void clean_dir(void)
{
	char string[200]; 
	char file[200];
	char message[200];
	DIR *linux_open_dir;
	struct dirent *linux_current_file;
	int x=0;
	
	getcwd(string,200);
	strcat(string,"/emfile-2k-0");
	sprintf(message,"\n\nunlinking emfile at this path: %s\n",string);
	print_message(3,message);
	unlink(string);
	

	linux_open_dir = opendir(linux_struct.root_path);
	if (linux_open_dir){
		for (x=0 ;NULL!=linux_current_file   ;x++)
		{
			linux_current_file =readdir(linux_open_dir);
			if (NULL!=linux_current_file){
				
				strcpy(file,linux_struct.root_path);
				strcat(file,linux_current_file->d_name);
				sprintf(message,"unlinking file %d\n",linux_current_file->d_name);
				print_message(3,message);
				unlink(file);
			}
		}
		unlink(linux_struct.root_path);
	}
	
}
