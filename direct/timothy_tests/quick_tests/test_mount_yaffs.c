#include "test_mount_yaffs.h"
#include "yaffsfs.h"
int mount_yaffs_test(void){
	int output=0;
	output=yaffs_mount(YAFFS_MOUNT_POINT);
	return output;
}

void mount_yaffs_test_clean(void){
	yaffs_unmount(YAFFS_MOUNT_POINT);
}
