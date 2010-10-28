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
#include <time.h>
#include <stdlib.h>

#include "yaffsfs.h"	/* it is in "yaffs2/direct/" link it in the Makefile */
#include "message_buffer.h"
#include "error_handler.h"
	
#define MAX_NUMBER_OF_OPENED_HANDLES 50
#define MAX_FILE_NAME_SIZE 51

typedef struct handle_regster_template{
	int handle[MAX_NUMBER_OF_OPENED_HANDLES];
	char path[MAX_NUMBER_OF_OPENED_HANDLES][100];
	int number_of_open_handles;
}handle_regster;


void init(char *yaffs_test_dir,char *yaffs_mount_dir,int argc, char *argv[]);	/*sets up yaffs and mounts yaffs */
void test(char *yaffs_test_dir);				/*contains the test code*/
void generate_random_string(char *ptr,int length_of_str);				/*generates a random string of letters to be used for a name*/
void join_paths(char *path1,char *path2,char *newpath );
void copy_array(char *from,char *to, unsigned int from_offset,unsigned int to_offset);
void stat_file(char *path);
void write_to_random_file(handle_regster *P_open_handles_array);
void close_random_file(handle_regster *P_open_handles_array);
void quit_program();
#endif
