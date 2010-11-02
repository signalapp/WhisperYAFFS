#include "test_yaffs_ftruncate.h"

int test_yaffs_ftruncate(void){
	int handle=test_open_file();
	if (handle>0){
		return yaffs_ftruncate(handle,FILE_SIZE_TRUNCATED );
	}
	else {
		printf("error opening file");
		return -1;
	}
}

int test_yaffs_ftruncate_clean(void){
	/* change file size back to orignal size */
	int handle=test_open_file();
	if (handle>0){
		return yaffs_ftruncate(handle,FILE_SIZE );
	}
	else {
		printf("error opening file in clean function");
		return -1;
	}

}
