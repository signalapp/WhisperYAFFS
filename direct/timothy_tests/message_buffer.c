#include "message_buffer.h"


void add_to_buffer(buffer *p_Buffer, char *message,char message_level){
	unsigned int x=0;
	/*move the head up one. the head always points at the last written data*/
	p_Buffer->head++;

//	printf("p_Buffer->tail=%d\n",p_Buffer->tail);
//	printf("p_Buffer->head=%d\n",p_Buffer->head);
	if (p_Buffer->head >=BUFFER_SIZE-1) {
//		printf("buffer overflow\n");
		p_Buffer->head -= (BUFFER_SIZE-1);	/*wrap the head around the buffer*/
//		printf("new p_Buffer->head=%d\n",p_Buffer->head);
	}	
	/*if the buffer is full then delete last entry by moving the tail*/
	if (p_Buffer->head==p_Buffer->tail){
//		printf("moving buffer tail from %d to ",p_Buffer->tail);
		p_Buffer->tail++;
		if (p_Buffer->tail >=BUFFER_SIZE) p_Buffer->tail -= BUFFER_SIZE;/*wrap the tail around the buffer*/
//		printf("%d\n",p_Buffer->tail);

	}




	
	

	p_Buffer->message_level[p_Buffer->head]=message_level; /*copy the message level*/
	x=p_Buffer->head;


	
	strcpy(p_Buffer->message[p_Buffer->head],message); /*copy the message*/
//	printf("copied %s into p_Buffer->message[p_Buffer->head]: %s\n",message,p_Buffer->message[p_Buffer->head]);
//	printf("extra %s\n",p_Buffer->message[p_Buffer->head]);

	if (p_Buffer->message_level[p_Buffer->head]<=DEBUG_LEVEL){
//		printf("printing buffer 1\n");
		/* the print buffer sfunction is not working this is just a quick fix*/		 
//		print_buffer(p_Buffer,1); /*if the debug level is higher enough then print the new message*/
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

