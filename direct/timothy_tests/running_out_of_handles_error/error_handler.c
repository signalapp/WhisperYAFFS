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

void yaffs_check_for_errors(char output, buffer *message_buffer,char error_message[],char success_message[])
{
	char dummy='0';
	if (output==-1)
	{
		add_to_buffer(message_buffer, "\nerror##########",MESSAGE_LEVEL_ERROR,PRINT);
		add_to_buffer(message_buffer, error_message,MESSAGE_LEVEL_ERROR,PRINT);
		add_to_buffer(message_buffer, "error_code: ",MESSAGE_LEVEL_ERROR,PRINT);
		if (MESSAGE_LEVEL_ERROR<=DEBUG_LEVEL)	printf("%d\n\n\n",yaffs_get_error());	/*cannot yet add int types to buffer. this is a quick fix*/ 		
		scanf("%c",dummy);
		//print_buffer(message_buffer,PRINT_ALL);
		
	}
	else{
		add_to_buffer(message_buffer, success_message,MESSAGE_LEVEL_BASIC_TASKS,PRINT);
	}
		
}
