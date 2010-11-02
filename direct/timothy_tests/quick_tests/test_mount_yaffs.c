#include "test_mount_yaffs.h"
#include "yaffsfs.h"
#include "lib.h"

int mount_yaffs_test(void){
	int output=0;
	output=yaffs_mount(YAFFS_MOUNT_POINT);
	/*printf("output %d",output);*/
	return output;
}

int mount_yaffs_test_clean(void){
	return 1;
	
}
