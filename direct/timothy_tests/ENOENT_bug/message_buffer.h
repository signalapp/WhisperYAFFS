/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __message_buffer__
#define __message_buffer__

#include <stdio.h>
#include <string.h>
#define PRINT 1
#define NPRINT 0
#define APPEND_MESSAGE 1
#define DO_NOT_APPEND_MESSAGE 0		
#define PRINT_ALL -1			/*this is used to print all of the messages in a buffer*/
#define BUFFER_MESSAGE_LENGTH 60		/*number of char in message*/
#define BUFFER_SIZE 50			/*number of messages in buffer*/
#define MESSAGE_LEVEL_ERROR 0
#define MESSAGE_LEVEL_BASIC_TASKS 1

#define LOG_FILE "log.txt"
typedef struct buffer_template{
	char message[BUFFER_SIZE][BUFFER_MESSAGE_LENGTH];
	int head;
	int tail;
	char message_level[BUFFER_SIZE];
}buffer; 
#include "error_handler.h"		/*include this for the debug level*/


void print_buffer(buffer *p_Buffer,int number_of_messages_to_print);		/*print messages in the buffer*/ 
/*wrapper functions for add_to_buffer_root_function*/
void add_to_buffer(buffer *p_Buffer, char *message,char message_level,char print);
void append_to_buffer(buffer *p_Buffer, char *message,char message_level,char print);
void add_int_to_buffer(buffer *p_Buffer, int num,char message_level,char print);
void append_int_to_buffer(buffer *p_Buffer, int num,char message_level,char print);

void add_to_buffer_root_function(buffer *p_Buffer, char *message,char message_level,char append,char print);
#endif
