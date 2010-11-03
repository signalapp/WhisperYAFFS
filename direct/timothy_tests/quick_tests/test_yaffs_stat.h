#ifndef __test_yaffs_stat_h__
#define __test_yaffs_stat_h__
#include "lib.h"
#include "yaffsfs.h"
#include "test_yaffs_open.h"
#include "test_yaffs_lseek.h"
int test_yaffs_stat(void);
int test_yaffs_stat_clean(void);
int yaffs_test_stat_mode(void);
int yaffs_test_stat_size(void);
#endif
