#include "test_yaffs_lseek.h"

static int handle=0;
int test_yaffs_lseek(void){
	handle=test_yaffs_open();
	char text[20]="\0";
	int output=0;
	if (handle>0){
		if (0==yaffs_lseek(handle, 0, SEEK_SET)){
			return 1;
		}
		else {
			printf("lseek returned a different position to the expeced position\n");
		}
	}
	else {
		printf("error opening file\n");
		return -1;
	}
	
}

int test_yaffs_lseek_clean(void){
	return yaffs_close(handle);	
}

