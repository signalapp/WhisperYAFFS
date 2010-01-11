
/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

/*
 * These are some dangly bits that need to be built to wrap up the rest of the yaffs test harness.
 *
 *
 * This is also the place where extra debugging stuff might end up.
 *
 */
 
#include "yaffsfs.h"
#include "yaffs_trace.h"

int simulate_power_failure;
int random_seed;

int yaffs_print_constants(void)
{
	printf( "O_CREAT........%d\n",O_CREAT);
	printf( "O_RDONLY.......%d\n",O_RDONLY);
	printf( "O_WRONLY.......%d\n",O_WRONLY);
	printf( "O_RDWR.........%d\n",O_RDWR);
	printf( "O_TRUNC........%d\n",O_TRUNC);

	printf( "sizeof(off_t)..%d\n",sizeof(off_t)); 
	return 0;
}

int yaffs_O_CREAT(void) { return O_CREAT;}
int yaffs_O_RDONLY(void) { return O_RDONLY;}
int yaffs_O_WRONLY(void) { return O_WRONLY;}
int yaffs_O_RDWR(void) { return O_RDWR;}
int yaffs_O_TRUNC(void) { return O_TRUNC;}

int yaffs_set_trace(unsigned int tm) { return yaffs_traceMask=tm; }
int yaffs_get_trace(void) { return yaffs_traceMask; }
