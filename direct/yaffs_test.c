/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */




#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "yaffsfs.h"

#include "nor_stress.h"



void BadUsage(void)
{
	printf("usage root_dir test_id\n");
	printf(" test_id: fw_update fw_update_init\n");
	exit(2);
}
int main(int argc, const char *argv[])
{
	yaffs_StartUp();
	if(argc == 3) {
		if(!strcmp(argv[2],"fw_update_init")){
			NorStressTestInitialise(argv[1]);
		}
		else if(!strcmp(argv[2],"fw_update")){
			NorStressTestRun(argv[1]);
		}
		else 
			BadUsage();
	}
	else
		BadUsage();
	return 0;
}
