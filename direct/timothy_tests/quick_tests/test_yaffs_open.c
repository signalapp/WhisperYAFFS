#include "test_yaffs_open.h"

static int handle=0;

int test_yaffs_open(void){

	handle=yaffs_open(FILE_PATH,O_CREAT | O_TRUNC| O_RDWR, FILE_MODE);
	return handle;
}


int test_yaffs_open_clean(void){
	if (handle >=0){
		return yaffs_close(handle);
	}
	else {
		return 1;	/* the file failed to open so there is no need to close it*/
	}
}

