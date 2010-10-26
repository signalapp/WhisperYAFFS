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
 * error_handler.c contains code for checking yaffs function calls for errors.
 */
#include "error_handler.h"


typedef struct error_codes_template {
  int code;
  char * text;  
}error_entry;

const error_entry error_list[] = {
	{ ENOMEM , "ENOMEM" },
	{ EBUSY , "EBUSY"},
	{ ENODEV , "ENODEV"},
	{ EINVAL , "EINVAL"},
	{ EBADF , "EBADF"},
	{ EACCES , "EACCES"},
	{ EXDEV , "EXDEV" },
	{ ENOENT , "ENOENT"},
	{ ENOSPC , "ENOSPC"},
	{ ERANGE , "ERANGE"},
	{ ENODATA, "ENODATA"},
	{ ENOTEMPTY, "ENOTEMPTY"},
	{ ENAMETOOLONG,"ENAMETOOLONG"},
	{ ENOMEM , "ENOMEM"},
	{ EEXIST , "EEXIST"},
	{ ENOTDIR , "ENOTDIR"},
	{ EISDIR , "EISDIR"},
	{ 0, NULL }
};

const char * error_to_str(int err)
{
	error_entry *e = error_list;
	if (err < 0) 
		err = -err;
	while(e->code && e->text){
		if(err == e->code)
			return e->text;
		e++;
	}
	return "Unknown error code";
}

void yaffs_check_for_errors(char output, buffer *message_buffer,char error_message[],char success_message[]){
	char dummy[10];
	unsigned int x=0;
	int yaffs_error=-1;
	char error_found=0;
	if (output==-1)
	{
		add_to_buffer(message_buffer, "\nerror##########",MESSAGE_LEVEL_ERROR,PRINT);
		add_to_buffer(message_buffer, error_message,MESSAGE_LEVEL_ERROR,PRINT);
		add_to_buffer(message_buffer, "error_code: ",MESSAGE_LEVEL_ERROR,PRINT);
		yaffs_error=yaffs_get_error();
		if (MESSAGE_LEVEL_ERROR<=DEBUG_LEVEL)	printf("%d\n",yaffs_error);	/*cannot yet add int types to buffer. this is a quick fix*/ 	
		add_to_buffer(message_buffer, error_to_str(yaffs_error),MESSAGE_LEVEL_ERROR,NPRINT);
		append_to_buffer(message_buffer, "\n\n",MESSAGE_LEVEL_ERROR,PRINT);	
				
		scanf("%c",dummy);	/*this line causes a segmentation fault. Need a better way of waiting for a key press*/
		//print_buffer(message_buffer,PRINT_ALL);
		
	}
	else{
		add_to_buffer(message_buffer, success_message,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
	}		
}


