#ifndef __message_buffer__
#define __message_buffer__

#include <stdio.h>
#include <string.h>

#define PRINT_ALL -1			/*this is used to print all of the messages in a buffer*/
#define BUFFER_MESSAGE_LENGTH 60		/*number of char in message*/
#define BUFFER_SIZE 50			/*number of messages in buffer*/
#define MESSAGE_LEVEL_ERROR 0
#define MESSAGE_LEVEL_BASIC_TASKS 1
typedef struct buffer_template{
	char message[BUFFER_SIZE][BUFFER_MESSAGE_LENGTH];
	int head;
	int tail;
	char message_level[BUFFER_SIZE];
}buffer; 
#include "error_handler.h"		/*include this for the debug level*/

void add_to_buffer(buffer *p_Buffer, char *message,char message_level);		/*code for buffer*/
void print_buffer(buffer *p_Buffer,int number_of_messages_to_print);		/*print messages in the buffer*/ 

#endif
