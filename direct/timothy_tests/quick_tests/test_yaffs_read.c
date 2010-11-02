#include "test_yaffs_write.h"

int test_yaffs_read(void){
	int handle=test_open_file();
	char text[20]="\0";
	int output=0;
	if (handle>0){
		output=yaffs_read(handle, text, FILE_TEXT_NBYTES);
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
		}
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_read_clean(void){
	/* need to reset the seek position*/
	
	return test_yaffs_lseek_to_beginning();
}
