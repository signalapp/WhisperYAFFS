/*yaffs_tester.h */
#ifndef __YAFFS_TESTER_H__
	#define __YAFFS_TESTER_H__

	#include <stdio.h>
	
	#include "yaffsfs.h"	/* it is in "yaffs2/direct/" link it in the Makefile */
	

	#define MAX_FILE_NAME_SIZE 51
	#define BUFFER_MESSAGE_LENGTH 50		/*number of char in message*/
	#define BUFFER_SIZE 50			/*number of messages in buffer*/
	typedef struct buffer_template
	{
		char message[BUFFER_SIZE][BUFFER_MESSAGE_LENGTH];
		char head;
		char tail;
	}buffer; 

	void init(char yaffs_test_dir[],char yaffs_mount_dir[]);	/*sets up yaffs and mounts yaffs */
	void test(char yaffs_test_dir[]);				/*contains the test code*/
	void generate_random_string(char *ptr);				/*generates a random string of letters to be used for a name*/
	void add_to_buffer(buffer *p_Buffer, char message[]);		/*code for buffer*/
	void print_buffer(buffer *p_Buffer);				/*print all of the messages in the buffer*/ 
#endif
