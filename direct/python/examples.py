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


from yaffsfs import *

def yaffs_ls(dname):
    if dname[-1] != "/": dname = dname + "/"
    dc = yaffs_opendir(dname)
    if dc != 0 :
        sep = yaffs_readdir(dc)
        while bool(sep):
            se = sep.contents
            fullname = dname + se.d_name
            #print fullname, " ", se.d_ino," ",ord(se.d_type)
            st = yaffs_stat_struct()
            result = yaffs_stat(fullname,byref(st))
            perms = st.st_mode & 0777
            isFile = True if st.st_mode & 0x8000 else False
            isDir  = True if st.st_mode & 0x4000 else False

            if isFile :
                print "File ",se.d_ino, hex(perms), st.st_size, fullname
            elif isDir :
                print "Dir  ",se.d_ino, hex(perms), fullname
                yaffs_ls(fullname)
            else :
            	print "Other (",hex(st.st_mode),") ",se.d_ino, hex(perms), fullname

            sep = yaffs_readdir(dc)
        yaffs_closedir(dc)
        return 0
    else:
        print "Could not open directory"
        return -1

def yaffs_mkfile(fname,fsize):
    fd = yaffs_open(fname,66, 0666)
    if fd >= 0:
        b = create_string_buffer("",1024)
        totalwrite=0
        while fsize > 0:
            thiswrite = 1024 if fsize > 1024 else fsize
            result = yaffs_write(fd,b,thiswrite)
            totalwrite += result
            fsize -= result
            if result != thiswrite:
                fsize= 0

        return totalwrite
    else :
        return -1

def yaffs_link_test(dir):
    fnamea = dir + '/aaa'
    fnameb = dir + '/bbb'
    yaffs_unlink(fnamea)
    fd = yaffs_open(fnamea,66,0666)
    yaffs_link(fnamea,fnameb)
    yaffs_ls(dir)
    yaffs_unlink(fnamea)
    yaffs_ls(dir)
    yaffs_unlink(fnameb)

def yaffs_o_excl_test(dir):
    fname = dir + '/aaa'
    yaffs_unlink(fname)
    fd = yaffs_open(fname, 66, 0666)
    yaffs_close(fd)
    print "Created ", fname, " result ", fd
    fdx = yaffs_open(fname, 0301, 0666)
    print "Attempt to create with O_EXCL existing file returned ", fdx
    yaffs_unlink(fname)
    fdx = yaffs_open(fname, 0301, 0666)
    print "Attempt to create with O_EXCL non-existing file returned ", fdx

    
root = "/yaffs2"

yaffs_start_up()
yaffs_mount(root)

yaffs_mkdir(root+"/dd",0666)

h = yaffs_open(root+"/dd/111",66,0666)
yaffs_close(h)

yaffs_ls(root)

