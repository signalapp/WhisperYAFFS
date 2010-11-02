#include "test_yaffs_truncate.h"

int test_yaffs_truncate(void){
	int handle=test_open_file();
	if (handle>0){
		return yaffs_truncate(FILE_PATH,FILE_SIZE_TRUNCATED );
	}
	else {
		printf("error opening file");
		return -1;
	}
}

int test_yaffs_truncate_clean(void){
	/* change file size back to orignal size */
	int handle=test_open_file();
	if (handle>0){
		return yaffs_truncate(FILE_PATH,FILE_SIZE );
	}
	else {
		printf("error opening file in clean function");
		return -1;
	}

}
