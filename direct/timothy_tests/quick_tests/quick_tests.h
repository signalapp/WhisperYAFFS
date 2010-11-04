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

#ifndef __quick_tests_h__
#define __quick_tests_h__
#include <stdio.h>

#include "test_mount_yaffs.h"

#include "test_yaffs_open.h"
#include "test_yaffs_open_EISDIR.h"
#include "test_yaffs_open_EEXIST.h"
#include "test_yaffs_open_ENOENT.h"
#include "test_yaffs_open_ENOTDIR.h"

#include "test_yaffs_close_EBADF.h"


#include "test_yaffs_unlinking.h"
#include "test_yaffs_ftruncate.h"
#include "test_yaffs_truncate.h"
#include "test_yaffs_write.h"
#include "test_yaffs_read.h"
#include "test_yaffs_lseek.h"
#include "test_yaffs_access.h"
#include "test_yaffs_stat.h"
#include "yaffsfs.h"
#include "yaffs_error_converter.h"
#include "lib.h"

typedef struct test {
	int (*p_function)(void);	/*pointer to test function*/
	int (*p_function_clean)(void);
	/*char pass_message[50]; will not need a pass message*/
	char *fail_message;	/*pointer to fail message, needs to include name of test*/
}test_template;


test_template test_list[]={
	{mount_yaffs_test,mount_yaffs_test_clean,"mount_yaffs_test"},
	{test_yaffs_open,test_yaffs_open_clean,"test_yaffs_open"},
	{test_yaffs_open_EISDIR,test_yaffs_open_EISDIR_clean,"test_yaffs_open_EISDIR"},
	{test_yaffs_open_EEXIST,test_yaffs_open_EEXIST_clean,"test_yaffs_open_EEXIST"},
//	{test_yaffs_open_ENOTDIR,test_yaffs_open_ENOTDIR_clean,"test_yaffs_open_ENOTDIR"},
//	{test_yaffs_open_ENOENT,test_yaffs_open_ENOENT_clean,"test_yaffs_open_ENOENT"},
	
	{test_yaffs_close_EBADF,test_yaffs_close_EBADF_clean,"test_yaffs_close_EBADF"},

	{test_yaffs_access,test_yaffs_access_clean,"test_yaffs_access"},
	{test_yaffs_unlinking, test_yaffs_unlinking_clean,"test_yaffs_unlinking"},

	{test_yaffs_lseek,test_yaffs_lseek_clean,"test_yaffs_lseek"},
	{test_yaffs_write,test_yaffs_write_clean,"test_yaffs_write"},
	{test_yaffs_read,test_yaffs_read_clean,"test_yaffs_read"},


	{test_yaffs_stat,test_yaffs_stat_clean,"test_yaffs_stat"},
	{test_yaffs_ftruncate,test_yaffs_ftruncate_clean,"test_yaffs_ftruncate"},
	{test_yaffs_truncate,test_yaffs_truncate_clean,"test_yaffs_truncate"}

	};

void init_quick_tests(void);
void quit_quick_tests(int exit_code);
#endif
