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



int random_seed;

void BadUsage(void)
{
	printf("usage root_dir test_id seed \n");
	printf(" test_id: fw_update fw_update_init\n");
	exit(2);
}
int main(int argc, const char *argv[])
{
	yaffs_StartUp();
	if(argc == 4) {
		random_seed = atoi(argv[3]);
		if(random_seed <= 0)
			BadUsage();
			
		if(!strcmp(argv[2],"fw_update_init")){
			NorStressTestInitialise(argv[1]);
		}
		else if(!strcmp(argv[2],"fw_update")){
			printf("Running stress on %s with seed %d\n",argv[1],random_seed);
			NorStressTestRun(argv[1]);
		}
		else 
			BadUsage();
	}
	else
		BadUsage();
	return 0;
}
