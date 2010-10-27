##
## YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
##
## Copyright (C) 2002-2010 Aleph One Ltd.
##   for Toby Churchill Ltd and Brightstar Engineering
##
## Created by Charles Manning <charles@aleph1.co.uk>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License version 2 as
## published by the Free Software Foundation.
##

from ctypes import *
cdll.LoadLibrary("./libyaffsfs.so")
ylib = CDLL("./libyaffsfs.so")

#const char * yaffs_error_to_str(int err);
yaffs_error_to_str = ylib.yaffs_error_to_str
yaffs_error_to_str.argtypes=[c_int]
yaffs_error_to_str.restype=c_char_p


#int yaffs_open_sharing(const YCHAR *path, int oflag, int mode, int sharingmode) ;
yaffs_open_sharing = ylib.yaffs_open_sharing
yaffs_open_sharing.argtypes = [ c_char_p, c_int, c_int, c_int]
yaffs_open_sharing.restype = c_int

#int yaffs_open(const YCHAR *path, int oflag, int mode) ;
yaffs_open = ylib.yaffs_open
yaffs_open.argtypes = [ c_char_p, c_int, c_int]
yaffs_open.restype = c_int

#int yaffs_close(int fd) ;
yaffs_close = ylib.yaffs_close
yaffs_close.argtypes = [ c_int]
yaffs_close.restype = c_int

#int yaffs_fsync(int fd) ;
yaffs_fsync = ylib.yaffs_fsync
yaffs_fsync.argtypes = [c_int]
yaffs_fsync.restype = c_int

#int yaffs_fdatasync(int fd) ;
yaffs_fdatasync = ylib.yaffs_fdatasync
yaffs_fdatasync.argtypes = [c_int]
yaffs_fdatasync.restype = c_int

#int yaffs_flush(int fd) ; /* same as yaffs_fsync() */
yaffs_flush = ylib.yaffs_flush
yaffs_flush.argtypes = [c_int]
yaffs_flush.restype = c_int

#int yaffs_access(const YCHAR *path, int amode);
yaffs_access=ylib.yaffs_access
yaffs_access.argtypes= [c_char_p,c_int]
yaffs_access.restype = c_int

#int yaffs_dup(int fd);
yaffs_dup = ylib.yaffs_dup
yaffs_dup.argtypes = [c_int]
yaffs_dup.restype = c_int

#int yaffs_read(int fd, void *buf, unsigned int nbyte) ;
yaffs_read=ylib.yaffs_read
yaffs_read.argtypes=[c_int,c_char_p,c_int]
yaffs_read.restype = c_int

#int yaffs_write(int fd, const void *buf, unsigned int nbyte) ;
yaffs_write=ylib.yaffs_write
yaffs_write.argtypes=[c_int,c_char_p,c_int]
yaffs_write.restype = c_int

#int yaffs_pread(int fd, void *buf, unsigned int nbyte, unsigned int offset);
yaffs_pread=ylib.yaffs_pread
yaffs_pread.argtypes=[c_int,c_char_p,c_int,c_int]
yaffs_pread.restype = c_int

#int yaffs_pwrite(int fd, const void *buf, unsigned int nbyte, unsigned int offset);
yaffs_pwrite=ylib.yaffs_pwrite
yaffs_pwrite.argtypes=[c_int,c_char_p,c_int,c_int]
yaffs_pwrite.restype = c_int

# off_t = 4 byte int
#off_t yaffs_lseek(int fd, off_t offset, int whence) ;
yaffs_lseek=ylib.yaffs_lseek
yaffs_lseek.argtypes= [c_int, c_int, c_int]
yaffs_lseek.restype= c_int

#int yaffs_truncate(const YCHAR *path, off_t newSize);
yaffs_truncate=ylib.yaffs_truncate
yaffs_truncate.argtypes = [c_char_p,c_int]
yaffs_truncate.restype = c_int

#int yaffs_ftruncate(int fd, off_t newSize);
yaffs_ftruncate = ylib.yaffs_ftruncate
yaffs_truncate.argtypes = [c_int,c_int]
yaffs_truncate.restype = c_int

#int yaffs_unlink(const YCHAR *path) ;
yaffs_unlink=ylib.yaffs_unlink
yaffs_unlink.argtypes = [c_char_p]
yaffs_unlink.restype = c_int

#int yaffs_rename(const YCHAR *oldPath, const YCHAR *newPath) ;
yaffs_rename= ylib.yaffs_rename
yaffs_rename.argtypes=[c_char_p, c_char_p]
yaffs_rename.restype = c_int

class yaffs_stat_struct(Structure):
    _fields_ = [
        ("st_dev", c_int),
        ("st_ino", c_int),
        ("st_mode", c_int),
        ("st_nlink", c_int),
        ("st_uid", c_int),
        ("st_gid", c_int),
        ("st_rdev", c_int),
        ("st_size", c_int),
        ("st_blksize", c_int),
        ("st_blocks", c_int),
        ("yst_atime", c_int),
        ("yst_mtime", c_int),
        ("yst_ctime", c_int)]

#int yaffs_stat(const YCHAR *path, struct yaffs_stat *buf) ;
yaffs_stat = ylib.yaffs_stat
yaffs_stat.argtypes=[c_char_p,POINTER(yaffs_stat_struct)]
yaffs_stat.restype=c_int

#int yaffs_lstat(const YCHAR *path, struct yaffs_stat *buf) ;
yaffs_lstat = ylib.yaffs_lstat
yaffs_lstat.argtypes=[c_char_p,POINTER(yaffs_stat_struct)]
yaffs_lstat.restype=c_int

#int yaffs_fstat(int fd, struct yaffs_stat *buf) ;
yaffs_fstat = ylib.yaffs_fstat
yaffs_fstat.argtypes=[c_int,POINTER(yaffs_stat_struct)]
yaffs_fstat.restype=c_int


#int yaffs_chmod(const YCHAR *path, mode_t mode);
yaffs_chmod = ylib.yaffs_chmod
yaffs_chmod.argtypes = [c_char_p, c_int]
yaffs_chmod.restype = c_int

#int yaffs_fchmod(int fd, mode_t mode); 
yaffs_fchmod = ylib.yaffs_fchmod
yaffs_fchmod.argtypes = [c_int, c_int]
yaffs_fchmod.restype = c_int

#int yaffs_mkdir(const YCHAR *path, mode_t mode) ;
yaffs_mkdir = ylib.yaffs_mkdir
yaffs_mkdir.argtypes = [c_char_p, c_int]
yaffs_mkdir.restype = c_int

#int yaffs_rmdir(const YCHAR *path) ;
yaffs_rmdir = ylib.yaffs_rmdir
yaffs_rmdir.argtypes = [c_char_p]
yaffs_rmdir.restype = c_int

class yaffs_dirent_struct(Structure):
    _fields_ = [
        ("d_ino", c_int),
        ("d_off", c_int),
        ("d_reclen", c_short),
        ("d_type", c_char),
        ("d_name", c_char * 257),
        ("d_dont_use", c_int)]

#yaffs_DIR *yaffs_opendir(const YCHAR *dirname) ;
yaffs_opendir = ylib.yaffs_opendir
yaffs_opendir.argtypes = [c_char_p]
yaffs_opendir.restype = c_int

#struct yaffs_dirent *yaffs_readdir(yaffs_DIR *dirp) ;
yaffs_readdir= ylib.yaffs_readdir
yaffs_readdir.argtypes=[c_int]
yaffs_readdir.restype=POINTER(yaffs_dirent_struct)

#void yaffs_rewinddir(yaffs_DIR *dirp) ;
yaffs_rewinddir = ylib.yaffs_rewinddir
yaffs_rewinddir.argtypes = [c_int]
yaffs_rewinddir.restype = c_int ###### Should be void

#int yaffs_closedir(yaffs_DIR *dirp) ;
yaffs_closedir = ylib.yaffs_closedir
yaffs_closedir.argtypes = [c_int]
yaffs_closedir.restype = c_int


#int yaffs_mount(const YCHAR *path) ;
yaffs_mount = ylib.yaffs_mount
yaffs_mount.argtypes = [c_char_p]
yaffs_mount.restype = c_int

#int yaffs_mount2(const YCHAR *path,int readOnly) ;
yaffs_mount2 = ylib.yaffs_mount2
yaffs_mount2.argtypes = [c_char_p, c_int]
yaffs_mount2.restype = c_int

#int yaffs_unmount(const YCHAR *path) ;
yaffs_unmount = ylib.yaffs_unmount
yaffs_unmount.argtypes = [c_char_p]
yaffs_unmount.restype = c_int

#int yaffs_unmount(const YCHAR *path, int force) ;
yaffs_unmount2 = ylib.yaffs_unmount2
yaffs_unmount2.argtypes = [c_char_p, c_int]
yaffs_unmount2.restype = c_int

#int yaffs_remount(const YCHAR *path, int force, int readOnly) ;
yaffs_remount = ylib.yaffs_remount
yaffs_remount.argtypes = [c_char_p, c_int, c_int]
yaffs_remount.restype = c_int

#int yaffs_sync(const YCHAR *path) ;
yaffs_sync = ylib.yaffs_sync
yaffs_sync.argtypes = [c_char_p]
yaffs_sync.restype = c_int

#int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath);
yaffs_symlink= ylib.yaffs_symlink
yaffs_symlink.argtypes= [c_char_p, c_char_p]
yaffs_symlink.restype = c_int

#int yaffs_readlink(const YCHAR *path, YCHAR *buf, int bufsiz); 
yaffs_readlink = ylib.yaffs_readlink
yaffs_readlink.argtypes = [c_char_p, c_char_p, c_int]
yaffs_readlink.restype = c_int

#int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath); 
yaffs_link = ylib.yaffs_link
yaffs_link.argtypes = [c_char_p, c_char_p]
yaffs_link.restype = c_int

#int yaffs_mknod(const YCHAR *pathname, mode_t mode, dev_t dev);
yaffs_mknod = ylib.yaffs_mknod
yaffs_mknod.argtypes = [c_char_p, c_int, c_int]
yaffs_mknod.restype = c_int

#loff_t yaffs_freespace(const YCHAR *path);
yaffs_freespace = ylib.yaffs_freespace
yaffs_freespace.argtypes = [c_char_p]
yaffs_freespace.restype = c_longlong

#loff_t yaffs_totalspace(const YCHAR *path);
yaffs_totalspace = ylib.yaffs_totalspace
yaffs_totalspace.argtypes = [c_char_p]
yaffs_totalspace.restype = c_longlong

#int yaffs_inodecount(const YCHAR *path);
yaffs_inodecount = ylib.yaffs_inodecount
yaffs_inodecount.argtypes = [c_char_p]
yaffs_inodecount.restype = c_int

#int yaffs_n_handles(const YCHAR *path)
yaffs_n_handles = ylib.yaffs_n_handles
yaffs_n_handles.argtypes = [c_char_p]
yaffs_n_handles.restype = c_int

#int yaffs_start_up(void)
yaffs_start_up = ylib.yaffs_start_up
yaffs_start_up.argtypes = []
yaffs_start_up.restype = c_int

#int yaffs_set_trace(unsigned int tm) { return yaffs_traceMask=tm; }
yaffs_set_trace=ylib.yaffs_set_trace
yaffs_set_trace.argtypes=[c_uint]
yaffs_set_trace.restype=c_uint

#int yaffs_get_trace(void) { return yaffs_traceMask; }
yaffs_get_trace=ylib.yaffs_get_trace
yaffs_get_trace.argtypes=[]
yaffs_get_trace.restypes=c_uint

#int yaffs_get_error(void)
yaffs_get_error=ylib.yaffs_get_error
yaffs_get_error.argtypes=[]
yaffs_get_error.restypes=c_int

yaffs_O_CREAT=ylib.yaffs_O_CREAT()
yaffs_O_RDONLY=ylib.yaffs_O_RDONLY()
yaffs_O_WRONLY=ylib.yaffs_O_WRONLY()
yaffs_O_RDWR=ylib.yaffs_O_RDWR()
yaffs_O_TRUNC=ylib.yaffs_O_TRUNC()


yaffs_S_IFMT=ylib.yaffs_S_IFMT()
yaffs_S_IFLNK= ylib.yaffs_S_IFLNK()
yaffs_S_IFDIR =ylib.yaffs_S_IFDIR()
yaffs_S_IFREG=ylib.yaffs_S_IFREG()
yaffs_S_IREAD=ylib.yaffs_S_IREAD()
yaffs_S_IWRITE=ylib.yaffs_S_IWRITE()
yaffs_S_IEXEC=ylib.yaffs_S_IEXEC()  
yaffs_XATTR_CREATE=ylib.yaffs_XATTR_CREATE()
yaffs_XATTR_REPLACE=ylib.yaffs_XATTR_REPLACE()
yaffs_S_IEXEC=ylib.yaffs_S_IEXEC()
