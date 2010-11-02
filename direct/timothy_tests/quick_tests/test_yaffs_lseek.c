#include "test_yaffs_lseek.h"

int test_yaffs_lseek(void){
	int handle=test_open_file();
	char text[20]="\0";
	int output=0;
	if (handle>0){
		return yaffs_lseek(handle, 0, SEEK_SET);
		/*
		if (output>0){
			if (text==FILE_TEXT){
				return 1;
			}
			else {
				printf("text does not match the the text that should be in the file\n");
			}
		}
		else{
			printf("error reading file");
		} */
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_lseek_clean(void){
	/* need to reset the seek position*/
	return test_yaffs_lseek_to_beginning();	
}
int test_yaffs_lseek_to_beginning(void){
	int handle=test_open_file();
	if (handle>0){
		return yaffs_lseek(handle, 0, SEEK_SET);
	}
	else {
		printf("error opening file\n");
		return -1;
	}
}
