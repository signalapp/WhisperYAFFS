#include "test_yaffs_stat.h"

int test_yaffs_stat(void){
	int mode=0;
	int size=0;
	mode =yaffs_test_stat_mode();
	printf("\nmode %o\n",mode);
	printf("expected mode %o \n",FILE_MODE);
	printf("anding together %o\n",FILE_MODE & mode);
	printf("%d\n",FILE_MODE == (FILE_MODE & mode));
	if (FILE_MODE == (FILE_MODE & mode)){
		mode=1;
	}
	else {
		printf("mode did not match expected file mode\n");
		return -1;
	}
	if (yaffs_test_stat_size()==FILE_SIZE){
		size=1;
	}
	else {
		printf("mode did not match expected file mode\n");
		return -1;
	}
	
	if (mode && size){
		return 1;
	}
	else {
		/* a test failed*/
		return -1;
	}

}

int test_yaffs_stat_clean(void){
	return 1;
}

int yaffs_test_stat_mode(void){
	struct yaffs_stat stat;
	int output=0;
	output=yaffs_stat(FILE_PATH, &stat);
	printf("output: %d\n",output);
	if (output>=0){
		return stat.st_mode;	
	}
	else {
		printf("failed to stat file\n") ;
		return -1;
	}
}

int yaffs_test_stat_size(void){
	struct yaffs_stat stat;
	int output=0;
	output=yaffs_stat(FILE_PATH, &stat); 
	if (output>=0){
		return stat.st_size;	
	}
	else {
		printf("failed to stat file\n") ;
		return -1;
	}
}


