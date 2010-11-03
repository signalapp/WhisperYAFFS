#include "test_yaffs_read.h"

int test_yaffs_read(void){
	int handle=test_open_file();
	char text[20]="\0";
	int output=0;
	printf("handle %d\n",handle);
	if (handle>0){
		output=yaffs_read(handle, text, FILE_TEXT_NBYTES);
		printf("yaffs_test_read output: %d\n",output);
		printf("text in file is: %s\n",text);
		if (output>0){ 
			if (text==FILE_TEXT){
				return 1;
			}
			else {
				printf("returned text does not match the the expected text that should be in the file\n");
				return -1;
			}
		}
		else{
			printf("error reading file\n");
			return -1;
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
