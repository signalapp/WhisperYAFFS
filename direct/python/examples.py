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
            if isDir :
                print "Dir  ",se.d_ino, hex(perms), fullname
                yaffs_ls(fullname)
                
            sep = yaffs_readdir(dc)
    else:
        print "Could not open directory"
        return -1


root = "/yaffs2"

yaffs_StartUp()
yaffs_mount(root)

yaffs_mkdir(root+"/dd",0666)

yaffs_open(root+"/dd/111",66,0666)

yaffs_ls(root)
