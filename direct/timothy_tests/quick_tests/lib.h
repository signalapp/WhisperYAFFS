#ifndef _lib_h__
#define _lib_h__


#include <string.h>
#define YAFFS_MOUNT_POINT "/yaffs2/"
#define FILE_NAME "foo"
#define FILE_SIZE 9

#define FILE_SIZE_TRUNCATED 100
#define FILE_TEXT "file foo"
#define FILE_TEXT_NBYTES 9

/* warning do not define anything as FILE because there seems to be a conflict with stdio.h */ 
#define FILE_PATH "/yaffs2/foo"

void join_paths(char *path1,char *path2,char *new_path );
#endif
