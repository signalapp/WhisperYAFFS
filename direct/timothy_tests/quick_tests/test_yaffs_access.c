#include "test_yaffs_access.h"

int test_yaffs_access(void){
		return yaffs_access(FILE_PATH,0);
}

int test_yaffs_access_clean(void){
	return 1;
}
