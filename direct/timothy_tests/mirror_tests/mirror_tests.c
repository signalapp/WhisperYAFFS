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
	4,
	{{"yaffs_test_open",yaffs_test_open},
	{"yaffs_test_truncate",yaffs_test_truncate},
	{"yaffs_test_unlink",yaffs_test_unlink},
	{"yaffs_test_write",yaffs_test_write}
	}
};

test_temp linux_tests={
	4,
	{{"linux_test_open",linux_test_open},
	{"linux_test_truncate",linux_test_truncate},
	{"linux_test_unlink",linux_test_unlink},
	{"linux_test_write",linux_test_write}
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
	//compare_linux_and_yaffs();
	yaffs_unmount("yaffs2");
	return 0;
}

void init(int argc, char *argv[])
{
	char dir[200];
	dir[0]='\0';
	int x=-1;
	char message[100];

	x=(unsigned)time(NULL);
	sprintf(message,"seeding srand with: %d\n",x);
	print_message(2,message);
	srand(x);
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
	int num_of_tests_before_check=1;
	char message[200];
	arg_temp args_struct;
	for (y=0;(y*num_of_tests_before_check)<num_of_random_tests;y++){
		for (x=0;x<num_of_tests_before_check && (x+(y*num_of_tests_before_check)<num_of_random_tests);x++) {
			errno=0;
			yaffs_set_error(0);
			test_id = select_test_id(yaffs_tests.num_of_tests);
			sprintf(message,"running test_id %d\n",test_id);
			print_message(3,message);
			generate_random_numbers(&args_struct);
			run_yaffs_test(test_id, &args_struct);

				//check_mode(&args_struct);
			
			run_linux_test(test_id, &args_struct);

			if (get_print_level()>=4){
				get_error_yaffs();
				get_error_linux();
			}
			
			if 	((abs(yaffs_get_error())!=abs(errno)) &&
				(abs(yaffs_get_error())!=EISDIR && abs(errno) != 0) &&
				(abs(yaffs_get_error())!=ENOENT && abs(errno) != EACCES)&&
				(abs(yaffs_get_error())!=EINVAL && abs(errno) != EBADF)
				){
				print_message(2,"\ndifference in returned errors######################################\n");
				get_error_yaffs();
				get_error_linux();
				if (get_exit_on_error()){ 
					exit(0);
				}
			}
		}
		//check_mode(&args_struct);	
		compare_linux_and_yaffs(&args_struct);
		//check_mode(&args_struct);

	} 
	compare_linux_and_yaffs(&args_struct);
}

int select_test_id(int test_len)
{
	int id=0;
	//printf("test_len = %d\n",test_len);
	id=(rand() % test_len );
	//printf("id %d\n",id);
	return id;

}

int check_mode(arg_temp *args_struct)
{
	char path[200];
	char message[200];
	int output=0;

	struct yaffs_stat yaffs_stat_struct;
	join_paths(yaffs_struct.root_path,args_struct->string1, path );
	sprintf(message,"\ntrying to stat to: %s\n",path);
	print_message(3,message);
	output=yaffs_stat(path,&yaffs_stat_struct);
	if (output < 0){
		sprintf(message,"failed to stat the file\n");
		print_message(3,message);
		get_error_yaffs();
	} else {
		sprintf(message,"stated the file\n");
		print_message(3,message);
		sprintf(message," yaffs file mode is %d\n",(yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE)));
		print_message(3,message);
		sprintf(message,"mode S_IREAD %d S_IWRITE %d\n",(yaffs_stat_struct.st_mode & S_IREAD),(yaffs_stat_struct.st_mode & S_IWRITE));
		print_message(3,message);	
	}
	return 1;
}

int check_mode_file(char *path)
{
	char message[200];
	int output=0;

	struct yaffs_stat yaffs_stat_struct;

	sprintf(message,"\ntrying to stat to: %s\n",path);
	print_message(3,message);
	output=yaffs_stat(path,&yaffs_stat_struct);
	if (output < 0){
		sprintf(message,"failed to stat the file\n");
		print_message(3,message);
		get_error_yaffs();
	} else {
		sprintf(message,"stated the file\n");
		print_message(3,message);
		sprintf(message," yaffs file mode is %d\n",(yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE)));
		print_message(3,message);
		sprintf(message,"mode S_IREAD %d S_IWRITE %d\n\n",(yaffs_stat_struct.st_mode & S_IREAD),(yaffs_stat_struct.st_mode & S_IWRITE));
		print_message(3,message);	
	}
	return 1;
}

int compare_linux_and_yaffs(arg_temp *args_struct)
{
	int x=0,y=0;
	char l_path[200];
	char y_path[200];
	char file_name[200];
	int exit_bool=0;
	int number_of_files_in_yaffs=0;
	int number_of_files_in_linux=0;
	char message[200];
	char **yaffs_file_list=NULL;
	char **linux_file_list=NULL;

	struct yaffs_stat yaffs_stat_struct;
	struct stat linux_stat_struct;
	yaffs_DIR *yaffs_open_dir;
	yaffs_dirent *yaffs_current_file;
	
	DIR *linux_open_dir;
	struct dirent *linux_current_file;
	
	l_path[0]='\0';
	y_path[0]='\0';
	file_name[0]='\0';
	message[0]='\0';
	print_message(2,"\n\n\n comparing folders\n");
//	check_mode_file("yaffs2/test/YY");
	//find out the number of files in the directory
	yaffs_open_dir = yaffs_opendir(yaffs_struct.root_path);
	if (yaffs_open_dir != NULL) {
		for (x=0;NULL!=yaffs_readdir(yaffs_open_dir);x++){}
		number_of_files_in_yaffs=x;
		sprintf(message,"number of files in yaffs dir= %d\n",number_of_files_in_yaffs);
		print_message(2,message);
		yaffs_rewinddir(yaffs_open_dir);
	} else {
		print_message(3,"failed to open yaffs test dir\n");
	}
	//create array
	yaffs_file_list= (char **)malloc(x*sizeof(char*));
	for (x=0;x<number_of_files_in_yaffs;x++) {
		yaffs_file_list[x]=malloc(200);
	}

	//check_mode_file("yaffs2/test/YY");
	//copy file names into array
	if (yaffs_open_dir !=NULL){
		yaffs_current_file =yaffs_readdir(yaffs_open_dir);
		for (x=0 ;yaffs_current_file;x++)
		{
			printf("x= %d \n",x);
			printf("yaffs_current_file->d_name = %s\n",yaffs_current_file->d_name);
			if (yaffs_current_file){
				strcpy(yaffs_file_list[x],yaffs_current_file->d_name);

			}
			yaffs_current_file =yaffs_readdir(yaffs_open_dir);
		}
		yaffs_closedir(yaffs_open_dir);
	} else {
		print_message(3,"failed to populate yaffs test list\n");
	}


	//find out the number of files in the directory
	linux_open_dir = opendir(linux_struct.root_path);
	if (linux_open_dir!=NULL){
		for (x=0;NULL!=readdir(linux_open_dir);x++){}
		number_of_files_in_linux=(x-2);
		sprintf(message,"number of files in linux dir= %d\n",(number_of_files_in_linux));
		print_message(2,message);
		//the -2 is because linux shows 2 extra files which are automaticly created. 
	
		rewinddir(linux_open_dir);
	} else {
		print_message(3,"failed to open linux test dir\n");
	}

	//create array
	linux_file_list= (char **)malloc(number_of_files_in_linux*sizeof(char*));
	
	for (x=0;x<number_of_files_in_linux;x++) {
		linux_file_list[x]=malloc(200);
	}

	//check_mode_file("yaffs2/test/YY");
	//copy file names into array
	if (linux_open_dir!=NULL){
		linux_current_file =readdir(linux_open_dir);
		for (x=0, y=0 ;NULL!=linux_current_file;x++)
		{

			if (NULL!=linux_current_file){
				strcpy(message,linux_current_file->d_name);
				print_message(7,"opened file: ");
				print_message(7,message);
				print_message(7,"\n");
			}
			if (NULL!=linux_current_file && 
				0!=strcmp(message,".")&&
				0!=strcmp(message,"..")){
				strcpy(file_name,linux_current_file->d_name);
				//sprintf("file opened: %s\n",linux_current_file->d_name);
				//print_message(3,message);
				print_message(7,"added file to list\n");
				strcpy(linux_file_list[y],file_name);
				sprintf(message,"file added to list: %s\n",linux_file_list[y]);
				print_message(7,message);
				y++;
			}
			linux_current_file =readdir(linux_open_dir);
		}
		closedir(linux_open_dir);
	} else {
		print_message(3,"failed to populate linux test dir\n");
	}
	

	//match the files in both folders
	for (x=0;x<number_of_files_in_yaffs;x++){
		sprintf(message,"\nsearching for yaffs file: %s\n",yaffs_file_list[x]);
		print_message(3,message);
		for (y=0;y<number_of_files_in_linux;y++){
			sprintf(message,"comparing to linux file: %s\n",linux_file_list[y]);
			print_message(7,message);

			if (0==strcmp(yaffs_file_list[x],linux_file_list[y])){
				sprintf(message,"file matched: %s\n",linux_file_list[y]);
				print_message(3,message);
				//check that the modes of both files are the same
				join_paths(yaffs_struct.root_path,yaffs_file_list[x],y_path);
				join_paths(linux_struct.root_path,linux_file_list[y],l_path);
				if (yaffs_stat(y_path,&yaffs_stat_struct)>=0&&
				stat(l_path,&linux_stat_struct)>=0){
					sprintf(message," yaffs file mode is %d\n",(yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE)));
					print_message(3,message);
					sprintf(message,"mode S_IREAD %d S_IWRITE %d\n",(yaffs_stat_struct.st_mode & S_IREAD),(yaffs_stat_struct.st_mode & S_IWRITE));
					print_message(3,message);				
					sprintf(message," linux file mode is %d\n",(linux_stat_struct.st_mode & (S_IREAD|S_IWRITE)));
					print_message(3,message);
					sprintf(message,"mode S_IREAD %d S_IWRITE %d\n",(linux_stat_struct.st_mode & S_IREAD),(linux_stat_struct.st_mode & S_IWRITE));
					print_message(3,message);
					if ((yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE))==
					( linux_stat_struct.st_mode & (S_IREAD|S_IWRITE))){
						print_message(2,"file modes match\n");
					} else {
						print_message(2,"file modes do not match\n");
						exit_bool=1;
					}
					linux_file_list[y][0]=NULL;
					yaffs_file_list[x][0]=NULL;
				} else {
					print_message(2,"failed to stat one of the files\n");
					get_error_yaffs();
					get_error_linux();
				}
				
				//read file contents
				
				
				break;
			}
		}
	}

	//print remaining files
	for (x=0;x<number_of_files_in_linux;x++){
		if (linux_file_list[x][0]!=NULL){
			sprintf(message,"unmatched file in linux: %s\n",linux_file_list[x]);
			print_message(2,message);
			exit_bool=1;
		}
	}
	for (x=0;x<number_of_files_in_yaffs;x++){
		if (yaffs_file_list[x][0]!=NULL){
			sprintf(message,"unmatched file in yaffs: %s\n",yaffs_file_list[x]);
			print_message(2,message);
			exit_bool=1;
		}
	}
	if (exit_bool==1&& get_exit_on_error()==1){
		print_message(2,"exiting program\n");
		exit(0);
	}

	for (x=0;x<number_of_files_in_yaffs;x++) {
		free(yaffs_file_list[x]);
	}
	free(yaffs_file_list);

	for (x=0;x<number_of_files_in_linux;x++) {
		free(linux_file_list[x]);
	}
	free(linux_file_list);


	//printf("file_name %s\n", yaffs_current_file->d_name);
//	generate_array_of_objects_in_yaffs(); 
//	generate_array_of_objects_in_linux();
	//first do a check to see if both sides have the same objects on both sides. 
	//then stat all of the files and compare size and mode
	//read the text of each file and compare them.
	
	//show the diffrences by printing them. 
	return 1;

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
		closedir(linux_open_dir);
		rmdir(linux_struct.root_path);
	}
	
}
