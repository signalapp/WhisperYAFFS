#include "test_yaffs_write.h"

int test_yaffs_write(void){
	int handle=test_open_file();
	if (handle>0){
		return yaffs_write(handle, FILE_TEXT, FILE_TEXT_NBYTES);
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_write_clean(void){
	if (0==test_yaffs_lseek_to_beginning()){
		return 1;
	}
	else {
		return -1;
	}
}
