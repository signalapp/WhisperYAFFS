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

void display_error(void)
{
	
}

void  generate_random_string(char *ptr,int length_of_str){
	unsigned int x;
	unsigned int length=((rand() %(length_of_str-3))+3);	/*creates a int with the number of charecters been between 1 and 51*/ 		
	char letter='\0';
	strcpy(ptr,"");
	//printf("generating string\n");
	//printf("string length is %d\n",length);
	for (x=0; x <= (length-2) &&length>2 ; x++)
	{
		//printf("x=%d\n",x);	
		/* keep generating a charecter until the charecter is legal*/
		while((letter=='\0' )||(letter=='/')||(letter=='\\')){
			letter=(rand() % 125-59)+58;	/*generate a number between 32 and 126 and uses it as a charecter (letter) */
		}	
		ptr[x]=letter;
		//printf("charecter generated is %c\n",ptr[x]);
	}
	ptr[x+1]='\0';	/*adds NULL charecter to turn it into a string*/
	
}

void join_paths(char *path1,char *path2,char *new_path )
{
	char message[100];
	print_message(3,"joining paths\n");
	sprintf(message,"path1: %s\n",path1);
	print_message(3,message);
	sprintf(message,"path2: %s\n",path2);
	print_message(3,message);
	strcpy(new_path,"");
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

void print_message(char print_level,char *message)
{
	if (print_level <= PRINT_LEVEL){
		printf(message);
	}
}
	


