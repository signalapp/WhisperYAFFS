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

#include "lib.h"

static int EXIT_ON_ERROR = 1;
static int  PRINT_LEVEL = 2;	//This sets the level of detail which is printed. There are 3 levels 0,1,2 and 3  
			//0 just prints the number of tests passed and failed.
			//1 is the basic print level. it will print the details of a failed test.
			//2 will print if a test passes and cleans. 
void set_print_level(int new_level)
{
	PRINT_LEVEL=new_level;
}

void set_exit_on_error(int num)
{
	EXIT_ON_ERROR=num;
}

int get_exit_on_error(void)
{
	return EXIT_ON_ERROR;
}


int set_up_ELOOP(void){
	int output1=1;
	int output2=1;
	int error =0;
	
	output1=yaffs_symlink(ELOOP_PATH,ELOOP2_PATH);
	if (output1 <0){
		error=yaffs_get_error();
		if (abs(error)==EEXIST){
			output1= 1;
		} else {
			output1=-1;
		}
	}
	
	output2=yaffs_symlink(ELOOP2_PATH,ELOOP_PATH);

	if (output2 <0){
		error=yaffs_get_error();
		if (abs(error)==EEXIST){
			output2= 1;
		} else {
			output2=-1;
		}
	}
	

	yaffs_set_error(0);	/*reset the last error to 0 */
	return (output1|output2);
}

void join_paths(char *path1,char *path2,char *new_path )
{

	//strcat(new_path,path1);	/*since all functions have this then pull it out*/
	if ( (path1[(sizeof(path1)/sizeof(char))-2]=='/') && path2[0]!='/') {
		/*paths are compatiable. concatanate them. note -2 is because of \0*/  
		strcat(new_path,path1);
		strcat(new_path,path2);		
		//char new_path[(sizeof(path1)/sizeof(char))+(sizeof(path2)/sizeof(char))];
		//strcpy(new_path,strcat(path1,path2)); 
		//return new_path;
	} else if ((path1[(sizeof(path1)/sizeof(char))-2]!='/') && path2[0]=='/') {
		/*paths are compatiable. concatanate them*/  
		strcat(new_path,path1);
		strcat(new_path,path2);		

	} else if ((path1[(sizeof(path1)/sizeof(char))-2]!='/') && path2[0]!='/') {
			/*need to add a "/". */  
		strcat(new_path,path1);
		strcat(new_path,"/");
		strcat(new_path,path2);

	} else if ((path1[(sizeof(path1)/sizeof(char))-2]=='/') && path2[0]=='/') {
		/*need to remove a "/". */
		/*yaffs does not mind the extra slash. */		
		strcat(new_path,path1);
		strcat(new_path,path2);

	} else {
		//error 
		//return -1;
	}
}

void print_message(char *message,char print_level)
{
	if (print_level <= PRINT_LEVEL){
		printf(message);
	}
}
	


