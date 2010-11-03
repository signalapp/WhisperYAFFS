#include "test_yaffs_open_ENOTDIR.h"

static int handle=0;
int test_yaffs_open_ENOTDIR(void){
	int output=0;
	int error_code=0;
	/*printf("path %s\n",path); */
	handle=yaffs_open("/nonexisting_dir/foo", O_TRUNC| O_RDWR,FILE_MODE );
	if (handle==-1){
		error_code=yaffs_get_error();
		if (abs(error_code)==ENOTDIR){
			return 1;
		}
		else {
			printf("different error than expected\n");
			return -1;
		}
	}
	else if (output >=0){
		printf("non existant directory opened.(which is a bad thing)\n");
		return -1;
	}

}
int test_yaffs_open_ENOTDIR_clean(void){
	if (handle >=0){
		return yaffs_close(handle);
	}
	else {
		return 1;	/* the file failed to open so there is no need to close it*/
	}
}

