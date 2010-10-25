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

#ifndef __YAFFS_TESTER_H__
	#define __YAFFS_TESTER_H__

#include <string.h>
#include <stdio.h>
	
#include "yaffsfs.h"	/* it is in "yaffs2/direct/" link it in the Makefile */
#include "message_buffer.h"
#include "error_handler.h"
	

#define MAX_FILE_NAME_SIZE 51

void init(char *yaffs_test_dir,char *yaffs_mount_dir);	/*sets up yaffs and mounts yaffs */
void test(char *yaffs_test_dir);				/*contains the test code*/
void generate_random_string(char *ptr);				/*generates a random string of letters to be used for a name*/
void join_paths(char *path1,char *path2,char *newpath );
void copy_array(char *from,char *to, unsigned int from_offset,unsigned int to_offset);
#endif
