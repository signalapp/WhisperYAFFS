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

#ifndef __mirror_tests_h__
#define __mirror_tests_h__

#include <stdio.h>
#include <unistd.h>	//used for getting the current directory. 
#include <string.h>
#include <errno.h>
#include "yaffsfs.h"
#include "lib.h"

#include "linux_test_open.h"
#include "yaffs_test_open.h"


#define LINUX 1
#define YAFFS 2





void init(int argc, char *argv[]);
int run_random_test(void);
int compare_linux_and_yaffs(void);
void get_error_yaffs(void);
int select_test_id(int test_len);
void generate_random_numbers(arg_temp *args_struct);
void run_yaffs_test(int id,arg_temp *args_struct);
void run_linux_test(int id,arg_temp *args_struct);
//void generate_array_of_objects_in_yaffs(void); 
//void generate_array_of_objects_in_linux(void);

#endif
