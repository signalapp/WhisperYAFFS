/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef _lib_h__
#define _lib_h__

#include <stdio.h>
#include <string.h>

#define EXIT_ON_ERROR 0
#define PRINT_LEVEL 2	//This sets the level of detail which is printed. There are 3 levels 0,1,2 and 3  
			//0 just prints the number of tests passed and failed.
			//1 is the basic print level. it will print the details of a failed test.
			//2 will print if a test passes and cleans. 

#define YAFFS_MOUNT_POINT "/yaffs2/"
#define FILE_NAME "foo"
#define FILE_SIZE 10

#define FILE_MODE (S_IREAD | S_IWRITE)
#define FILE_SIZE_TRUNCATED 100
#define FILE_TEXT "file foo "	/* keep space at end of string */
#define FILE_TEXT_NBYTES 10

/* warning do not define anything as FILE because there seems to be a conflict with stdio.h */ 
#define FILE_PATH "/yaffs2/foo"

void join_paths(char *path1,char *path2,char *new_path );
void print_message(char *message,char print_level);
#endif
