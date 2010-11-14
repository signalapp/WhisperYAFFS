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

#include "test_yaffs_lseek.h"

static int handle = -1;

int test_yaffs_lseek(void)
{
	int output=0;
	handle = test_yaffs_open();


	if (handle >= 0){
		output = yaffs_lseek(handle, 0, SEEK_SET);
		if (output == 0){
			return 1;
		} else {
			if (output<0){
				print_message("lseek failed to seek\n",2);
			} else {
				print_message("lseek returned a different position to the expeced position\n", 2);
				return -1;
			}
		}
	} else {
		print_message("error opening file\n", 2);
		return -1;
	}
	
}

int test_yaffs_lseek_clean(void)
{
	if (handle>=0){
		return yaffs_close(handle);
	} else {
		return 1; /* no handle was opened so there is no need to close a handle */
	}

}



