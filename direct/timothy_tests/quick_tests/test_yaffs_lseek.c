#include "test_yaffs_lseek.h"

int test_yaffs_lseek(void){
	int handle=test_open_file();
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
	/* need to reset the seek position*/
	return test_yaffs_lseek_to_beginning();	
}
int test_yaffs_lseek_to_beginning(void){
	int handle=test_open_file();
	if (handle>0){
		if (0==yaffs_lseek(handle, 0, SEEK_SET)){	/*lseek returns the position of the seeker */
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
