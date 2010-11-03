#include "test_yaffs_unlinking.h"

int test_yaffs_unlinking(void){
	int output=yaffs_unlink(FILE_PATH);
	if (output>=0){
		return (-test_yaffs_access());	/*return negative access. we do not want the file to be there*/
	}
	else {
		printf("failed to unlink file\n") ;
		return -1;
	}
}

int test_yaffs_unlinking_clean(void){
	return test_yaffs_open_file();
}
