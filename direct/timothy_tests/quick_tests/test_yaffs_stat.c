/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "test_yaffs_stat.h"

int test_yaffs_stat(void)
{
	char message[100];
	int mode=0;
	int size=0;
	message[0]='\0';
	struct yaffs_stat stat;

	return yaffs_stat(FILE_PATH, &stat);
}

int test_yaffs_stat_clean(void)
{
	return 1;
}

int yaffs_test_stat_mode(void)
{
	struct yaffs_stat stat;
	int output=0;
	output=yaffs_stat(FILE_PATH, &stat);
	//printf("output: %d\n",output);
	if (output>=0){
		return stat.st_mode;	
	} else {
		print_message("failed to stat file mode\n",2) ;
		return -1;
	}
}

int yaffs_test_stat_size(void){
	struct yaffs_stat stat;
	int output=0;
	output=yaffs_stat(FILE_PATH, &stat); 
	if (output>=0){
		return stat.st_size;	
	} else {
		print_message("failed to stat file size\n",2) ;
		return -1;
	}
}


