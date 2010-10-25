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
 * message_buffer.c contains code for a message buffer .
 */

#include "message_buffer.h"

void append_to_buffer(buffer *p_Buffer, char *message,char message_level,char print){
	/*wrapper function for add_to_buffer_root_function*/
	add_to_buffer_root_function(p_Buffer,message, message_level,APPEND_MESSAGE,print);
}

void add_to_buffer(buffer *p_Buffer, char *message,char message_level,char print){
	/*wrapper function for add_to_buffer_root_function*/
	add_to_buffer_root_function(p_Buffer,message, message_level,DO_NOT_APPEND_MESSAGE,print);
}

void add_to_buffer_root_function(buffer *p_Buffer, char *message,char message_level,char append,char print){

	if (append==APPEND_MESSAGE){	/* append current message*/
		strncat(p_Buffer->message[p_Buffer->head],message,BUFFER_MESSAGE_LENGTH);
	}
	else {

		/*move the head up one. the head always points at the last written data*/
		p_Buffer->head++;

		/*printf("p_Buffer->tail=%d\n",p_Buffer->tail);*/
		/*printf("p_Buffer->head=%d\n",p_Buffer->head);*/
		if (p_Buffer->head >=BUFFER_SIZE-1) {
			/*printf("buffer overflow\n");*/
			p_Buffer->head -= (BUFFER_SIZE-1);	/*wrap the head around the buffer*/
			/*printf("new p_Buffer->head=%d\n",p_Buffer->head);*/
		}	
		/*if the buffer is full then delete last entry by moving the tail*/
		if (p_Buffer->head==p_Buffer->tail){
			/*printf("moving buffer tail from %d to ",p_Buffer->tail);*/
			p_Buffer->tail++;	
			if (p_Buffer->tail >=BUFFER_SIZE) p_Buffer->tail -= BUFFER_SIZE;/*wrap the tail around the buffer*/
			/*printf("%d\n",p_Buffer->tail);*/

		}

		p_Buffer->message_level[p_Buffer->head]=message_level; /*copy the message level*/
		strncpy(p_Buffer->message[p_Buffer->head],message,BUFFER_MESSAGE_LENGTH); /*copy the message*/
		/*printf("copied %s into p_Buffer->message[p_Buffer->head]: %s\n",message,p_Buffer->message[p_Buffer->head]);
		printf("extra %s\n",p_Buffer->message[p_Buffer->head]);*/
	}
	if ((p_Buffer->message_level[p_Buffer->head]<=DEBUG_LEVEL)&& (print==PRINT)){
		/*printf("printing buffer 1\n");
		// the print buffer function is not working this is just a quick fix		 
		print_buffer(p_Buffer,1); //if the debug level is higher enough then print the new message  
		*/
		printf("%s\n",p_Buffer->message[p_Buffer->head]);
	}
}



void print_buffer(buffer *p_Buffer, int number_of_messages_to_print){	
	int x=0;
	int i=0;
	printf("print buffer\n");
	printf("buffer head:%d\n",p_Buffer->head);
	printf("buffer tail:%d\n",p_Buffer->tail);

	if (number_of_messages_to_print==PRINT_ALL) number_of_messages_to_print=BUFFER_SIZE;
//	printf("number_of_messages_to_print=%d\n",number_of_messages_to_print);
	for (i=0,x=0; (x>=p_Buffer->tail) && (i<number_of_messages_to_print); i++, x--){
//		printf("printing buffer\n");
//		printf("x:%d\n",x);
		if (x<0) x = BUFFER_SIZE-1;		/*wrap x around buffer*/
		printf("%s\n",p_Buffer->message[p_Buffer->head]);
		printf("printed buffer\n");
	}

}

