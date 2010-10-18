/*yaffs_tester.c*/
#include "yaffs_tester.h"


int random_seed;
int simulate_power_failure = 0;


buffer message_buffer;	/*create  message_buffer */



int main()
{	
	char yaffs_test_dir[]="/yaffs2/test_dir";
	char yaffs_mount_dir[]="/yaffs2/";
	printf("welcome to the yaffs tester\n");
	init(yaffs_test_dir,yaffs_mount_dir);
	test(yaffs_test_dir);

	yaffs_unmount(yaffs_mount_dir);
	return 0;
}

void add_to_buffer(buffer *p_Buffer, char message[])
{
	if (p_Buffer->head+1==p_Buffer->tail)
	{
		p_Buffer->tail++;
		if (p_Buffer->tail >BUFFER_SIZE-1) p_Buffer->tail -= BUFFER_SIZE-1;
		
	}
	/*move the head up one. the head always points at the last witten data*/
	p_Buffer->head++;
	if (p_Buffer->head >BUFFER_SIZE) p_Buffer->head -= BUFFER_SIZE;

	strcpy(p_Buffer->buffer[p_Buffer->head],message);

}
void print_buffer(buffer *p_Buffer)
{
	for (x=p_Buffer->head; x>=p_Buffer->tail; x--)
	{
		
	}

}


void init(char yaffs_test_dir[],char yaffs_mount_dir[])
{
	yaffs_StartUp();
    	yaffs_mount(yaffs_mount_dir);
	if (yaffs_access(yaffs_test_dir,0))
	{
		yaffs_mkdir(yaffs_test_dir,S_IREAD | S_IWRITE);
	}
}

void yaffs_check_for_errors(char output)
{
	if (output==-1)
	{
		printf("error####");
		print_buffer(message_buffer);
	}
}

void test(char yaffs_test_dir[])
{
	char name[MAX_FILE_NAME_SIZE +3];
	unsigned int x;
	while(1)
	{

		generate_random_string(name);
		printf("the length of the string is %d \n",strlen(name)); 
		printf("generated string is:");
		for (x=0; x <= (strlen(name)-1)&&x<=MAX_FILE_NAME_SIZE ; x++)
		{
//			printf("x=%d\n",x);
			printf("%c ",name[x]);
		}
		printf("\n");
	}
}
void  generate_random_string(char *ptr)
{
	unsigned int x;
	unsigned int length=((rand() %MAX_FILE_NAME_SIZE)+1);	/*creates a int with the number of charecters been between 1 and 51*/ 		
	printf("generating string\n");
	printf("string length is %d\n",length);
	for (x=0; x <= (length-2) &&length>2 ; x++)
	{
//		printf("x=%d\n",x);	
		ptr[x]=(rand() % 126-32)+32;	/*generate a number between 32 and 126 and uses it as a charecter (letter) */
//		printf("charecter generated is %c\n",ptr[x]);
	}
	ptr[x+1]=0;	/*adds NULL charecter to turn it into a string*/
	
}

