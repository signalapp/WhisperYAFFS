#include "test_open_file.h"
#include "lib.h"
#include "yaffsfs.h"



int test_open_file(void){
	
	int output=0;

	/*printf("path %s\n",path); */
	output=yaffs_open(FILE_PATH,O_CREAT | O_TRUNC| O_RDWR, FILE_MODE);

	return output;
}
int test_open_file_clean(void){
	return 1;
}

