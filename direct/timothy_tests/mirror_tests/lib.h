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
#include "yaffsfs.h"


typedef struct arg_temp2{
	char char1;
	char char2;
	char string1[100];
	char string2[100];
	int int1;
	int int2;
} arg_temp;


typedef struct test_dir_temp
{
	int type_of_test; //used to tell if it is LINUX of YAFFS
	char root_path[200];
} test_dir;
test_dir yaffs_struct,linux_struct;



void generate_random_string(char *ptr,int length_of_str);
void join_paths(char *path1,char *path2,char *new_path );
void print_message(char print_level, char *message);
void set_print_level(int new_level);
void set_exit_on_error(int num);
int get_exit_on_error(void);
void display_error(void);

#endif
