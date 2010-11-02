#include "test_yaffs_unlinking.h"

int test_yaffs_unlinking(void){
	int output=yaffs_unlink(FILE_PATH);
	return output;
}

int test_yaffs_unlinking_clean(void){
	return test_open_file();
}
