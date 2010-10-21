#ifndef __error_handler_h__
#define __error_handler_h__
#include <stdio.h>
#include "message_buffer.h"
#include "yaffsfs.h"
#define DEBUG_LEVEL 5 /*set the debug level. this is used to display the relevent debug messages*/
void yaffs_check_for_errors(char output, buffer *message_buffer, char error_message[], char success_message[]);
#endif
