#include "test_yaffs_open_file_ENOENT.h"

static int handle=0;
int test_yaffs_open_file_ENOENT(void){
	int output=0;
	int error_code=0;
	/*printf("path %s\n",path); */
	handle=yaffs_open("/yaffs2/non_existant_file", O_TRUNC| O_RDWR,FILE_MODE );
	if (handle==-1){
		error_code=yaffs_get_error();
		if (error_code==ENOENT){
			return 1;
		}
	}
	else if (output >=0){
		printf("non existant file opened.(which is a bad thing)\n");
		return -1;
	}

}
int test_yaffs_open_file_ENOENT_clean(void){
	return 1;
}

