/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

/*
 * Header file for using yaffs in an application via
 * a direct interface.
 */


#ifndef __YAFFSFS_H__
#define __YAFFSFS_H__

#include "yaffscfg.h"
#include "yportenv.h"


//typedef long off_t;
//typedef long dev_t;
//typedef unsigned long mode_t;


#ifndef NAME_MAX
#define NAME_MAX	256
#endif


#ifdef CONFIG_YAFFSFS_PROVIDE_VALUES

#ifndef O_RDONLY
#define O_RDONLY        00
#endif

#ifndef O_WRONLY
#define O_WRONLY	01
#endif

#ifndef O_RDWR
#define O_RDWR		02
#endif

#ifndef O_CREAT		
#define O_CREAT 	0100
#endif

#ifndef O_EXCL
#define O_EXCL		0200
#endif

#ifndef O_TRUNC
#define O_TRUNC		01000
#endif

#ifndef O_APPEND
#define O_APPEND	02000
#endif

#ifndef SEEK_SET
#define SEEK_SET	0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR	1
#endif

#ifndef SEEK_END
#define SEEK_END	2
#endif

#ifndef EBUSY
#define EBUSY	16
#endif

#ifndef ENODEV
#define ENODEV	19
#endif

#ifndef EINVAL
#define EINVAL	22
#endif

#ifndef EBADF
#define EBADF	9
#endif

#ifndef EACCES
#define EACCES	13
#endif

#ifndef EXDEV	
#define EXDEV	18
#endif

#ifndef ENOENT
#define ENOENT	2
#endif

#ifndef ENOSPC
#define ENOSPC	28
#endif

#ifndef ENOTEMPTY
#define ENOTEMPTY 39
#endif

#ifndef ENOMEM
#define ENOMEM 12
#endif

#ifndef EEXIST
#define EEXIST 17
#endif

#ifndef ENOTDIR
#define ENOTDIR 20
#endif

#ifndef EISDIR
#define EISDIR 21
#endif


// Mode flags

#ifndef S_IFMT
#define S_IFMT		0170000
#endif

#ifndef S_IFLNK
#define S_IFLNK		0120000
#endif

#ifndef S_IFDIR
#define S_IFDIR		0040000
#endif

#ifndef S_IFREG
#define S_IFREG		0100000
#endif

#ifndef S_IREAD 
#define S_IREAD		0000400
#endif

#ifndef S_IWRITE
#define	S_IWRITE	0000200
#endif

#ifndef S_IEXEC
#define	S_IEXEC	0000100
#endif

#ifndef R_OK
#define R_OK	4
#define W_OK	2
#define X_OK	1
#define F_OK	0
#endif

#else
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif


struct yaffs_dirent{
    long d_ino;                 /* inode number */
    off_t d_off;                /* offset to this dirent */
    unsigned short d_reclen;    /* length of this d_name */
    YUCHAR  d_type;	/* type of this record */
    YCHAR d_name [NAME_MAX+1];   /* file name (null-terminated) */
    unsigned d_dont_use;	/* debug pointer, not for public consumption */
};

typedef struct yaffs_dirent yaffs_dirent;


typedef struct __opaque yaffs_DIR;



struct yaffs_stat{
    int		      st_dev;      /* device */
    int           st_ino;      /* inode */
    unsigned      st_mode;     /* protection */
    int           st_nlink;    /* number of hard links */
    int           st_uid;      /* user ID of owner */
    int           st_gid;      /* group ID of owner */
    unsigned      st_rdev;     /* device type (if inode device) */
    off_t         st_size;     /* total size, in bytes */
    unsigned long st_blksize;  /* blocksize for filesystem I/O */
    unsigned long st_blocks;   /* number of blocks allocated */
#ifdef CONFIG_YAFFS_WINCE
	/* Special 64-bit times for WinCE */
	unsigned long yst_wince_atime[2];
	unsigned long yst_wince_mtime[2];
	unsigned long yst_wince_ctime[2];
#else
	unsigned long yst_atime;    /* time of last access */
    unsigned long yst_mtime;    /* time of last modification */
    unsigned long yst_ctime;    /* time of last change */
#endif
};

int yaffs_open(const YCHAR *path, int oflag, int mode) ;
int yaffs_close(int fd) ;
int yaffs_flush(int fd) ;

int yaffs_access(const YCHAR *path, int amode);

int yaffs_dup(int fd);

int yaffs_read(int fd, void *buf, unsigned int nbyte) ;
int yaffs_write(int fd, const void *buf, unsigned int nbyte) ;

int yaffs_pread(int fd, void *buf, unsigned int nbyte, unsigned int offset);
int yaffs_pwrite(int fd, const void *buf, unsigned int nbyte, unsigned int offset);

off_t yaffs_lseek(int fd, off_t offset, int whence) ;

int yaffs_truncate(const YCHAR *path, off_t newSize);
int yaffs_ftruncate(int fd, off_t newSize);

int yaffs_unlink(const YCHAR *path) ;
int yaffs_rename(const YCHAR *oldPath, const YCHAR *newPath) ;

int yaffs_stat(const YCHAR *path, struct yaffs_stat *buf) ;
int yaffs_lstat(const YCHAR *path, struct yaffs_stat *buf) ;
int yaffs_fstat(int fd, struct yaffs_stat *buf) ;

#ifdef CONFIG_YAFFS_WINCE

int yaffs_set_wince_times(int fd, const unsigned *wctime, const unsigned *watime, const unsigned *wmtime);
int yaffs_get_wince_times(int fd, unsigned *wctime, unsigned *watime, unsigned *wmtime);

#endif

int yaffs_chmod(const YCHAR *path, mode_t mode); 
int yaffs_fchmod(int fd, mode_t mode); 

int yaffs_mkdir(const YCHAR *path, mode_t mode) ;
int yaffs_rmdir(const YCHAR *path) ;

yaffs_DIR *yaffs_opendir(const YCHAR *dirname) ;
struct yaffs_dirent *yaffs_readdir(yaffs_DIR *dirp) ;
void yaffs_rewinddir(yaffs_DIR *dirp) ;
int yaffs_closedir(yaffs_DIR *dirp) ;

int yaffs_mount(const YCHAR *path) ;
int yaffs_unmount(const YCHAR *path) ;

int yaffs_sync(const YCHAR *path) ;

int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath); 
int yaffs_readlink(const YCHAR *path, YCHAR *buf, int bufsiz); 

int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath); 
int yaffs_mknod(const YCHAR *pathname, mode_t mode, dev_t dev);

loff_t yaffs_freespace(const YCHAR *path);
loff_t yaffs_totalspace(const YCHAR *path);

int yaffs_inodecount(const YCHAR *path);


void yaffs_initialise(yaffsfs_DeviceConfiguration *configList);

int yaffs_StartUp(void);

#endif


