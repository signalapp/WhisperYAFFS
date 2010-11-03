#include "test_yaffs_ftruncate.h"

static int handle=0;
int test_yaffs_ftruncate(void){
	handle=test_yaffs_open();
	if (handle>0){
		return yaffs_ftruncate(handle,FILE_SIZE_TRUNCATED );
	}
	else {
		printf("error opening file\n");
		return -1;
	}
}

int test_yaffs_ftruncate_clean(void){
	/* change file size back to orignal size */
	int output=0;
	if (handle>0){
		output=yaffs_ftruncate(handle,FILE_SIZE );
		if (output>=0){
			return yaffs_close(handle);
		}
		else {
			printf("failed to truncate file\n");
			return -1;
		}
	}
	else {
		printf("error opening file in clean function\n");
		return -1;
	}
	
}
