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

/*
 * yaffs_tester.c designed to stress test yaffs2 direct.
 */


#include "yaffs_tester.h"



int random_seed;
int simulate_power_failure = 0;


buffer message_buffer;	/*create  message_buffer */

char yaffs_test_dir[] ="/yaffs2/test_dir";	/*the path to the directory where all of the testing will take place*/
char yaffs_mount_dir[]="/yaffs2/";		/*the path to the mount point which yaffs will mount*/


int main(int argc, char *argv[]){	
	
	
	init(yaffs_test_dir,yaffs_mount_dir,argc,argv);
	test(yaffs_test_dir);
	yaffs_unmount(yaffs_mount_dir);
	return 0;
}



void init(char *yaffs_test_dir,char *yaffs_mount_dir,int argc, char *argv[]){
	char output=0;
	int x=0;
	int seed=-1;
	FILE *log_handle;
	/*these variables are already set to zero, but it is better not to take chances*/
	message_buffer.head=0;				 
	message_buffer.tail=0;


	log_handle=fopen(LOG_FILE,"w");
	if (log_handle!=NULL){
		fputs("log file for yaffs tester\n",log_handle);
		fclose(log_handle);
	}
	add_to_buffer(&message_buffer,"welcome to the yaffs tester",MESSAGE_LEVEL_BASIC_TASKS,PRINT);/* print boot up message*/	
	yaffs_start_up();
    	yaffs_mount(yaffs_mount_dir);
	for (x=0;x<argc;x++){
//		add_to_buffer(&message_buffer,"argv ",MESSAGE_LEVEL_BASIC_TASKS,PRINT);
//		add_to_buffer(&message_buffer,argv[x],MESSAGE_LEVEL_BASIC_TASKS,PRINT);
		if (strcmp("-seed",argv[x])==0){			/*warning only compares the length of the strings, quick fix*/
			seed= atoi(argv[x+1]);
			/*add_to_buffer(&message_buffer,"setting seed to ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
			append_int_to_buffer(&message_buffer,seed,MESSAGE_LEVEL_BASIC_TASKS,NPRINT);			
			append_to_buffer(&message_buffer,"\n",MESSAGE_LEVEL_BASIC_TASKS,PRINT);*/
		}
	}
	if (seed==-1){
		seed=time(NULL);
		srand(seed); 
	}
	else {
	srand(seed);
	}
	add_to_buffer(&message_buffer,"setting seed to ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
	append_int_to_buffer(&message_buffer,seed,MESSAGE_LEVEL_BASIC_TASKS,PRINT);/* print boot up message*/	

	if (yaffs_access(yaffs_test_dir,0))	/* if the test folder does not exist then create it */
	{
		add_to_buffer(&message_buffer,"creating dir: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer,yaffs_test_dir,MESSAGE_LEVEL_BASIC_TASKS,PRINT);	
		output=yaffs_mkdir(yaffs_test_dir,S_IREAD | S_IWRITE);
		yaffs_check_for_errors(output, &message_buffer,"could not create dir","created dir\n\n");
	}
	
}

void quit_program(){
	yaffs_unmount(yaffs_mount_dir);
	exit(1);
}
void join_paths(char *path1,char *path2,char *new_path ){

/*	printf("strlen path1:%d\n",strlen(path1));
	printf("strlen path2:%d\n",strlen(path2));
	printf("path1; %s\n",path1);
*/
	add_to_buffer(&message_buffer, "joining paths:",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
	append_to_buffer(&message_buffer,path1,MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
	append_to_buffer(&message_buffer, " and ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
	append_to_buffer(&message_buffer, path2,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
	if ( (path1[(sizeof(path1)/sizeof(char))-2]=='/') && path2[0]!='/') {
		/*paths are compatiable. concatanate them. note -2 is because of \0*/  
		strcat(new_path,path1);
		strcat(new_path,path2);		
		//char new_path[(sizeof(path1)/sizeof(char))+(sizeof(path2)/sizeof(char))];
		//strcpy(new_path,strcat(path1,path2)); 
		//return new_path;
	}	
	else if ((path1[(sizeof(path1)/sizeof(char))-2]!='/') && path2[0]=='/') {
		/*paths are compatiable. concatanate them*/  
		strcat(new_path,path1);
		strcat(new_path,path2);		
		//char new_path[(sizeof(path1)/sizeof(char))+(sizeof(path2)/sizeof(char))];
		//strcpy(new_path,strcat(path1,path2)); 
		//return new_path;
	}
	else if ((path1[(sizeof(path1)/sizeof(char))-2]!='/') && path2[0]!='/') {
			/*need to add a "/". */  
		strcat(new_path,path1);
		strcat(new_path,"/");
		strcat(new_path,path2);
		//strcpy(new_path,strcat(path1,strcat("/\0",path2)));

#if 0
		copy_array(path1,new_path,0,0);
		copy_array('\0',new_path,0,(sizeof(path1)/sizeof(char)));
		copy_array(path2,new_path,0,(sizeof(path1)/sizeof(char))+1);
 old method now trying to use copy_array
		//char new_path[(sizeof(path1)/sizeof(char))+(sizeof(path2)/sizeof(char))+1];
		for (x=0;x<=(sizeof(path1)/sizeof(char))-1;x++){ 
			new_path[x]=path1[x];
		}
		new_path[x+1]='/';
		for (x=(sizeof(path1)/sizeof(char)) ,i=0 ;i<=(sizeof(path2)/sizeof(char));x++,i++){ 
			new_path[x]=path2[i]; 
		}
#endif

		//return new_path;
	}
	else if ((path1[(sizeof(path1)/sizeof(char))-2]=='/') && path2[0]=='/') {
		/*need to remove a "/". */
		/*yaffs does not mind the extra slash. */
		//char new_path[(sizeof(path1)/sizeof(char))+(sizeof(path2)/sizeof(char))-1];
		
		strcat(new_path,path1);
		strcat(new_path,path2);
		//strcpy(new_path,strcat(path1,strncat("",path2,(sizeof(path1)/sizeof(char))-1))); 
		//return new_path;
	} 
	else{
		//error 
		//return -1;
	}
}

void open_random_file(char *yaffs_test_dir, handle_regster *P_open_handles_array){
	int output=0;
	int x=0;
	char name[MAX_FILE_NAME_SIZE+3 ]="apple\0";
	char path[MAX_FILE_NAME_SIZE+strlen(yaffs_test_dir)];
	path[0]='\0';// this should clear the path
	add_to_buffer(&message_buffer,"\n\number of opened handles: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
	append_int_to_buffer(&message_buffer,P_open_handles_array->number_of_open_handles,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
	if (P_open_handles_array->number_of_open_handles<MAX_NUMBER_OF_OPENED_HANDLES)
	{
		generate_random_string(name,MAX_FILE_NAME_SIZE);
		printf("before %d %d %d\n",strlen(yaffs_test_dir),strlen(name),strlen(path));
		join_paths(yaffs_test_dir,name,path);//bug###################### here
		printf("after %d %d %d\n",strlen(yaffs_test_dir),strlen(name),strlen(path));
		add_to_buffer(&message_buffer,"trying to open file: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer,path,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
		if (yaffs_access(path,0)==0){
			stat_file(path);
		}
		else {
			add_to_buffer(&message_buffer,"file does not exists, creating file",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		}

		output=yaffs_open(path,O_CREAT | O_TRUNC| O_RDWR, S_IREAD | S_IWRITE);
		x=0;
		for (x=0;P_open_handles_array->handle[x]!=-3 && x<MAX_NUMBER_OF_OPENED_HANDLES;x++){}	/*find an empty handle*/	 

		add_to_buffer(&message_buffer,"handle array id ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,x,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
			
		//for (x=0;handle_pointers[x]!=NULL;x++){}
		P_open_handles_array->handle[x]=output;
		P_open_handles_array->path[x][0]='\0';
		strcat(P_open_handles_array->path[x],path);
		add_to_buffer(&message_buffer,"yaffs handle: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,output,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
		add_to_buffer(&message_buffer,"stored handle: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
	
		//yaffs_open will return a null pointer if it cannot open a file. check for errors will not work.			
		yaffs_check_for_errors(output, &message_buffer,"failed to open file","opened file");
		
		P_open_handles_array->number_of_open_handles++; 
	}
	else close_random_file(P_open_handles_array);
 
}

void write_to_random_file(handle_regster *P_open_handles_array){
	int number_of_lines_of_text=0;
	int length=100;
	char text[length+1];
	text[0]='\0';
	int seek=0;
	int x=0;
	int output=0;
	if (P_open_handles_array->number_of_open_handles>0){
		
		while (P_open_handles_array->handle[x]==-3){		/*find a random open handle*/
			x=rand() % (MAX_NUMBER_OF_OPENED_HANDLES-1);
		}
		add_to_buffer(&message_buffer,"\n\ntrying to write to ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer,P_open_handles_array->path[x],MESSAGE_LEVEL_BASIC_TASKS,PRINT);
		
		stat_file(P_open_handles_array->path[x]);
		number_of_lines_of_text=rand() %1000;
		add_to_buffer(&message_buffer,"writing  ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,number_of_lines_of_text,MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer," lines of text",MESSAGE_LEVEL_BASIC_TASKS,PRINT);

		
		for (;number_of_lines_of_text>0;number_of_lines_of_text--)
		{
			generate_random_string(text,length);
			seek=rand()%1000;
			add_to_buffer(&message_buffer,"trying to seek to  ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
			append_int_to_buffer(&message_buffer,seek,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
			output=yaffs_lseek(P_open_handles_array->handle[x],seek,SEEK_SET);
			yaffs_check_for_errors(output, &message_buffer,"failed to seek","seeked file");
			add_to_buffer(&message_buffer,"trying to write to file",MESSAGE_LEVEL_BASIC_TASKS,PRINT);
			output=yaffs_write(P_open_handles_array->handle[x], text, strlen(text));
			yaffs_check_for_errors(output, &message_buffer,"failed to write text","wrote text");
					
		}	
	 }
}
 
void close_random_file(handle_regster *P_open_handles_array){
	/*run out of space on the handle pointer array*/	
	/*make space*/
	int x=0;
	int output=0;
		while (P_open_handles_array->handle[x]==-3){		
			x=rand() % (MAX_NUMBER_OF_OPENED_HANDLES-1);
		}
		add_to_buffer(&message_buffer,"\n\ntrying to close file: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer,P_open_handles_array->path[x],MESSAGE_LEVEL_BASIC_TASKS,PRINT);
		add_to_buffer(&message_buffer,"file handle: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,P_open_handles_array->handle[x],MESSAGE_LEVEL_BASIC_TASKS,PRINT);

		stat_file(P_open_handles_array->path[x]);
		output=yaffs_close(P_open_handles_array->handle[x]);

		if (output==-1) yaffs_check_for_errors(output, &message_buffer,"failed to close file","closed file");
		else {
			yaffs_check_for_errors(output, &message_buffer,"failed to close file","closed file");
			P_open_handles_array->handle[x]=-3;
			P_open_handles_array->path[x][0]='\0';
			P_open_handles_array->number_of_open_handles--;
		}
}

void stat_file(char *path){
	int output=0;
	struct yaffs_stat stat;
	if (yaffs_access(path,0)==0){
		add_to_buffer(&message_buffer,"file exists, trying to stat: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		add_to_buffer(&message_buffer,path,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
		output=yaffs_lstat(path,&stat);
		yaffs_check_for_errors(output, &message_buffer,"failed to stat file","statted file");
		//stat.st_ino,(int)stat.st_size,stat.st_mode
		add_to_buffer(&message_buffer,"yaffs inode: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,stat.st_ino,MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer," file size: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,(int)stat.st_size,MESSAGE_LEVEL_BASIC_TASKS,NPRINT);			
		append_to_buffer(&message_buffer," file mode: ",MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_int_to_buffer(&message_buffer,stat.st_mode,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
	}
	else{
		add_to_buffer(&message_buffer,path,MESSAGE_LEVEL_BASIC_TASKS,NPRINT);
		append_to_buffer(&message_buffer," does not exist,could not stat",MESSAGE_LEVEL_BASIC_TASKS,PRINT);
	}
}

void test(char*yaffs_test_dir){
	struct yaffs_stat stat;
	int output=0;
	//char name[MAX_FILE_NAME_SIZE+3 ]="apple\0";
	//char path[MAX_FILE_NAME_SIZE];
	handle_regster open_handles_array;
	//int handle_pointers[MAX_NUMBER_OF_OPENED_HANDLES];
	//int number_of_opened_handles=0;
	int x=0;
	
		
	open_handles_array.number_of_open_handles=0;
	for (x=0;x<MAX_NUMBER_OF_OPENED_HANDLES;x++){
		open_handles_array.handle[x]=-3;
		open_handles_array.path[x][0]='\0';

	}
	while(1)
	{
		x=rand() % 2;
		switch(x){
			case 0 :open_random_file(yaffs_test_dir,&open_handles_array);break;
			case 1 :write_to_random_file(&open_handles_array);break;
			case 2 :close_random_file(&open_handles_array);break;
		}
	}
}
void  generate_random_string(char *ptr,int length_of_str){
	unsigned int x;
	unsigned int length=((rand() %length_of_str)+1);	/*creates a int with the number of charecters been between 1 and 51*/ 		
	char letter='\0';

	//printf("generating string\n");
	//printf("string length is %d\n",length);
	for (x=0; x <= (length-2) &&length>2 ; x++)
	{
		//printf("x=%d\n",x);	
		/* keep generating a charecter until the charecter is legal*/
		while((letter=='\0' )||(letter=='/')||(letter=='\\')){
			letter=(rand() % 126-32)+32;	/*generate a number between 32 and 126 and uses it as a charecter (letter) */
		}	
		ptr[x]=letter;
		//printf("charecter generated is %c\n",ptr[x]);
	}
	ptr[x+1]='\0';	/*adds NULL charecter to turn it into a string*/
	
}

