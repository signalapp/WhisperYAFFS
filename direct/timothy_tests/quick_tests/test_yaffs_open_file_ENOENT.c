#include "test_yaffs_open_file_ENOENT.h"


int test_yaffs_open_file_ENOENT(void){
	
	int output=0;

	/*printf("path %s\n",path); */
	output=yaffs_open(FILE_PATH, O_TRUNC| O_RDWR, "/yaffs2/non_existant_file");
	if (output==ENOENT){
		return output;
	}
	else if (output >=0){
		printf("non existant file opened.(which is a bad thing)\n");
		return -1;
	}
}
int test_yaffs_open_file_ENOENT_clean(void){
	return 1;
}

