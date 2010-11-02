#include "test_yaffs_write.h"

int test_yaffs_write(void){
	int handle=test_open_file();
	if (handle>0){
		return yaffs_write(handle, FILE_TEXT, FILE_TEXT_NBYTES);
	}
	else {
		printf("error opening file");
		return -1;
	}
	
}

int test_yaffs_write_clean(void){
	return 1;
}
