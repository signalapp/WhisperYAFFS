#ifndef __quick_tests_h__
#define __quick_tests_h__
#include <stdio.h>

#include "test_mount_yaffs.h"
#include "test_open_file.h"
#include "test_yaffs_unlinking.h"
#include "test_yaffs_ftruncate.h"
#include "test_yaffs_truncate.h"
#include "test_yaffs_write.h"
#include "test_yaffs_read.h"
#include "test_yaffs_lseek.h"
#include "test_yaffs_access.h"
#include "test_yaffs_stat.h"
#define YAFFS_MOUNT_POINT "/yaffs2/"

void init_quick_tests(void);
int dummy_test_pass(void);
int dummy_test_fail(void);
void quit_quick_tests(int exit_code);
#endif
