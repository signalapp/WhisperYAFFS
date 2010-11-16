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

#include "test_yaffs_mount.h"
#include "test_yaffs_mount_ENODEV.h"
#include "test_yaffs_mount_ENAMETOOLONG.h"
#include "test_yaffs_mount_EBUSY.h"

#include "test_yaffs_unmount.h"
#include "test_yaffs_unmount_ENODEV.h"
#include "test_yaffs_unmount_ENAMETOOLONG.h"
#include "test_yaffs_unmount_EBUSY.h"

#include "test_yaffs_open.h"
#include "test_yaffs_open_EISDIR.h"
#include "test_yaffs_open_EEXIST.h"
#include "test_yaffs_open_ENOENT.h"
#include "test_yaffs_open_ENOTDIR.h"
#include "test_yaffs_open_ENAMETOOLONG.h"
#include "test_yaffs_open_EINVAL.h"
#include "test_yaffs_open_EINVAL2.h"

#include "test_yaffs_close_EBADF.h"


#include "test_yaffs_unlink.h"
#include "test_yaffs_unlink_EISDIR.h"
#include "test_yaffs_unlink_ENOENT.h"
#include "test_yaffs_unlink_ENAMETOOLONG.h"
#include "test_yaffs_unlink_ENOTDIR.h"
#include "test_yaffs_unlink_ENOENT.h"

#include "test_yaffs_ftruncate.h"
#include "test_yaffs_ftruncate_EBADF.h"
#include "test_yaffs_ftruncate_EINVAL.h"
#include "test_yaffs_ftruncate_EFBIG.h"


#include "test_yaffs_truncate.h"
#include "test_yaffs_truncate_ENOTDIR.h"
#include "test_yaffs_truncate_EISDIR.h"
#include "test_yaffs_truncate_ENOENT.h"
#include "test_yaffs_truncate_EINVAL.h"
#include "test_yaffs_truncate_EFBIG.h"

#include "test_yaffs_write.h"
#include "test_yaffs_write_EBADF.h"
#include "test_yaffs_write_EFBIG.h"

#include "test_yaffs_read.h"
#include "test_yaffs_read_EBADF.h"
#include "test_yaffs_read_EINVAL.h"

#include "test_yaffs_lseek.h"
#include "test_yaffs_lseek_EBADF.h"
#include "test_yaffs_lseek_EINVAL.h"
#include "test_yaffs_lseek_EFBIG.h"

#include "test_yaffs_access.h"
#include "test_yaffs_access_EINVAL.h"
#include "test_yaffs_access_ENOTDIR.h"
#include "test_yaffs_access_ENOENT.h"

#include "test_yaffs_stat.h"
#include "test_yaffs_stat_ENOENT.h"
#include "test_yaffs_stat_ENOTDIR.h"

#include "test_yaffs_fstat.h"
#include "test_yaffs_fstat_EBADF.h"

#include "test_yaffs_chmod.h"
#include "test_yaffs_chmod_ENOENT.h"
#include "test_yaffs_chmod_ENOTDIR.h"
#include "test_yaffs_chmod_EINVAL.h"


#include "test_yaffs_fchmod.h"
#include "test_yaffs_fchmod_EBADF.h"
#include "test_yaffs_fchmod_EINVAL.h"

#include "test_yaffs_fsync.h"
#include "test_yaffs_fsync_EBADF.h"

#include "test_yaffs_fdatasync.h"
#include "test_yaffs_fdatasync_EBADF.h"

#include "test_yaffs_mkdir.h"
#include "test_yaffs_mkdir_EEXIST.h"
#include "test_yaffs_mkdir_ENOTDIR.h"

#include "test_yaffs_symlink.h"
#include "test_yaffs_symlink_ENOTDIR.h"


#include "yaffsfs.h"
#include "yaffs_error_converter.h"
#include "lib.h"




typedef struct test {
	int (*p_function)(void);	/*pointer to test function*/
	int (*p_function_clean)(void);
	/*char pass_message[50]; will not need a pass message*/
	char *name_of_test;	/*pointer to fail message, needs to include name of test*/
}test_template;


test_template test_list[]={
	{test_yaffs_mount,test_yaffs_mount_clean,"test_yaffs_mount"},
	{test_yaffs_mount_ENODEV,test_yaffs_mount_ENODEV_clean,"test_yaffs_mount_ENODEV"},
	{test_yaffs_mount_ENAMETOOLONG,test_yaffs_mount_ENAMETOOLONG_clean,"test_yaffs_mount_ENAMETOOLONG"},
	{test_yaffs_mount_EBUSY,test_yaffs_mount_EBUSY_clean,"test_yaffs_mount_EBUSY"},

	{test_yaffs_unmount,test_yaffs_unmount_clean,"test_yaffs_unmount"},
	{test_yaffs_unmount_ENODEV,test_yaffs_unmount_ENODEV_clean,"test_yaffs_unmount_ENODEV"},
	{test_yaffs_unmount_ENAMETOOLONG,test_yaffs_unmount_ENAMETOOLONG_clean,"test_yaffs_unmount_ENAMETOOLONG"},
	{test_yaffs_unmount_EBUSY,test_yaffs_unmount_EBUSY_clean,"test_yaffs_unmount_EBUSY"},

	{test_yaffs_open,test_yaffs_open_clean,"test_yaffs_open"},
	{test_yaffs_open_EISDIR,test_yaffs_open_EISDIR_clean,"test_yaffs_open_EISDIR"},
	{test_yaffs_open_EEXIST,test_yaffs_open_EEXIST_clean,"test_yaffs_open_EEXIST"},
	{test_yaffs_open_ENOTDIR,test_yaffs_open_ENOTDIR_clean,"test_yaffs_open_ENOTDIR"},
	{test_yaffs_open_ENOENT,test_yaffs_open_ENOENT_clean,"test_yaffs_open_ENOENT"},
	{test_yaffs_open_ENAMETOOLONG,test_yaffs_open_ENAMETOOLONG_clean,"test_yaffs_open_ENAMETOOLONG"},
	{test_yaffs_open_EINVAL,test_yaffs_open_EINVAL_clean,"test_yaffs_open_EINVAL"},
	{test_yaffs_open_EINVAL2,test_yaffs_open_EINVAL2_clean,"test_yaffs_open_EINVAL2"},
	
	{test_yaffs_close_EBADF,test_yaffs_close_EBADF_clean,"test_yaffs_close_EBADF"},

	{test_yaffs_access,test_yaffs_access_clean,"test_yaffs_access"},
	{test_yaffs_access_EINVAL,test_yaffs_access_EINVAL_clean,"test_yaffs_access_EINVAL"},
	{test_yaffs_access_ENOTDIR,test_yaffs_access_ENOTDIR_clean,"test_yaffs_access_ENOTDIR"},
	{test_yaffs_access_ENOENT,test_yaffs_access_ENOENT_clean,"test_yaffs_access_ENOENT"},

	{test_yaffs_unlink, test_yaffs_unlink_clean,"test_yaffs_unlink"},
	{test_yaffs_unlink_EISDIR,test_yaffs_unlink_EISDIR_clean,"test_yaffs_unlink_EISDIR"},
	{test_yaffs_unlink_ENOENT,test_yaffs_unlink_ENOENT_clean,"test_yaffs_unlink_ENOENT"},
	{test_yaffs_unlink_ENAMETOOLONG,test_yaffs_unlink_ENAMETOOLONG_clean,"test_yaffs_unlink_ENAMETOOLONG"},
	{test_yaffs_unlink_ENOTDIR,test_yaffs_unlink_ENOTDIR_clean,"test_yaffs_unlink_ENOTDIR"},
	{test_yaffs_unlink_ENOENT,test_yaffs_unlink_ENOENT_clean,"test_yaffs_unlink_ENOENT"},


	{test_yaffs_lseek,test_yaffs_lseek_clean,"test_yaffs_lseek"},
	{test_yaffs_lseek_EBADF,test_yaffs_lseek_EBADF_clean,"test_yaffs_lseek_EBADF"},
	{test_yaffs_lseek_EINVAL,test_yaffs_lseek_EINVAL_clean,"test_yaffs_lseek_EINVAL"},
	{test_yaffs_lseek_EFBIG,test_yaffs_lseek_EFBIG_clean,"test_yaffs_lseek_EFBIG"},

	{test_yaffs_write,test_yaffs_write_clean,"test_yaffs_write"},
	{test_yaffs_write_EBADF,test_yaffs_write_EBADF_clean,"test_yaffs_write_EBADF"},
//	{test_yaffs_write_EFBIG,test_yaffs_write_EFBIG_clean,"test_yaffs_write_EFBIG"},

	{test_yaffs_read,test_yaffs_read_clean,"test_yaffs_read"},
	{test_yaffs_read_EBADF,test_yaffs_read_EBADF_clean,"test_yaffs_read_EBADF"},
	{test_yaffs_read_EINVAL,test_yaffs_read_EINVAL_clean,"test_yaffs_read_EINVAL"},

	{test_yaffs_stat,test_yaffs_stat_clean,"test_yaffs_stat"},
	{test_yaffs_stat_ENOENT,test_yaffs_stat_ENOENT_clean,"test_yaffs_stat_ENOENT"},
	{test_yaffs_stat_ENOTDIR,test_yaffs_stat_ENOTDIR_clean,"test_yaffs_stat_ENOTDIR"},

	{test_yaffs_fstat,test_yaffs_fstat_clean,"test_yaffs_fstat"},
	{test_yaffs_fstat_EBADF,test_yaffs_fstat_EBADF_clean,"test_yaffs_fstat_EBADF"},

	{test_yaffs_ftruncate,test_yaffs_ftruncate_clean,"test_yaffs_ftruncate"},
	{test_yaffs_ftruncate_EBADF,test_yaffs_ftruncate_EBADF_clean,"test_yaffs_ftruncate_EBADF"},
	{test_yaffs_ftruncate_EINVAL,test_yaffs_ftruncate_EINVAL_clean,"test_yaffs_ftruncate_EINVAL"},
	{test_yaffs_ftruncate_EFBIG,test_yaffs_ftruncate_EFBIG_clean,"test_yaffs_ftruncate_EFBIG"},

	{test_yaffs_truncate,test_yaffs_truncate_clean,"test_yaffs_truncate"},
	{test_yaffs_truncate_ENOTDIR,test_yaffs_truncate_ENOTDIR_clean,"test_yaffs_truncate_ENOTDIR"},
	{test_yaffs_truncate_EISDIR,test_yaffs_truncate_EISDIR_clean,"test_yaffs_truncate_EISDIR"},
	{test_yaffs_truncate_EINVAL,test_yaffs_truncate_EINVAL_clean,"test_yaffs_truncate_EINVAL"},
	{test_yaffs_truncate_ENOENT,test_yaffs_truncate_ENOENT_clean,"test_yaffs_truncate_ENOENT"},
	{test_yaffs_truncate_EFBIG,test_yaffs_truncate_EFBIG_clean,"test_yaffs_truncate_EFBIG"},

	{test_yaffs_chmod,test_yaffs_chmod_clean,"test_yaffs_chmod"},
	{test_yaffs_chmod_ENOENT,test_yaffs_chmod_ENOENT_clean,"test_yaffs_chmod_ENOENT"},
	{test_yaffs_chmod_ENOTDIR,test_yaffs_chmod_ENOTDIR_clean,"test_yaffs_chmod_ENOTDIR"},
	{test_yaffs_chmod_EINVAL,test_yaffs_chmod_EINVAL_clean,"test_yaffs_chmod_EINVAL"},

	{test_yaffs_fchmod,test_yaffs_fchmod_clean,"test_yaffs_fchmod"},
	{test_yaffs_fchmod_EBADF,test_yaffs_fchmod_EBADF_clean,"test_yaffs_fchmod_EBADF"},
	{test_yaffs_fchmod_EINVAL,test_yaffs_fchmod_EINVAL_clean,"test_yaffs_fchmod_EINVAL"},

	{test_yaffs_fsync,test_yaffs_fsync_clean,"test_yaffs_fsync"},
	{test_yaffs_fsync_EBADF,test_yaffs_fsync_EBADF_clean,"test_yaffs_fsync_EBADF"},

	{test_yaffs_fdatasync,test_yaffs_fdatasync_clean,"test_yaffs_fdatasync"},
	{test_yaffs_fdatasync_EBADF,test_yaffs_fdatasync_EBADF_clean,"test_yaffs_fdatasync_EBADF"},

	{test_yaffs_mkdir,test_yaffs_mkdir_clean,"test_yaffs_mkdir"},
	{test_yaffs_mkdir_EEXIST,test_yaffs_mkdir_EEXIST_clean,"test_yaffs_mkdir_EEXIST"},
	{test_yaffs_mkdir_ENOTDIR,test_yaffs_mkdir_ENOTDIR_clean,"test_yaffs_mkdir_ENOTDIR"},

	{test_yaffs_symlink,test_yaffs_symlink_clean,"test_yaffs_symlink"},
	{test_yaffs_symlink_ENOTDIR,test_yaffs_symlink_ENOTDIR_clean,"test_yaffs_symlink_ENOTDIR"}



	};

void init_quick_tests(int argc, char *argv[]);
void quit_quick_tests(int exit_code);
void get_error(void);
#endif
