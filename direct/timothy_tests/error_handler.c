#include "error_handler.h"

void yaffs_check_for_errors(char output, buffer *message_buffer,char error_message[],char success_message[])
{
	if (output==-1)
	{
		add_to_buffer(message_buffer, "error##########",MESSAGE_LEVEL_ERROR);
		add_to_buffer(message_buffer, error_message,MESSAGE_LEVEL_ERROR);
		add_to_buffer(message_buffer, "error_code",MESSAGE_LEVEL_ERROR);
		 printf("%d\n",yaffs_get_error());		

		//print_buffer(message_buffer,PRINT_ALL);
	
	}
	else{
		add_to_buffer(message_buffer, success_message,MESSAGE_LEVEL_BASIC_TASKS);
	}
		
}
