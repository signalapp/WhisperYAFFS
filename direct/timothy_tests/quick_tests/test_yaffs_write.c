#include "test_yaffs_write.h"

static int handle=0;
int test_yaffs_write(void){
	handle=test_yaffs_open();
	if (handle>0){
		return yaffs_write(handle, FILE_TEXT, FILE_TEXT_NBYTES);
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_write_clean(void){
	return yaffs_close(handle);
}
