##
## YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
##
## Copyright (C) 2002-2010 Aleph One Ltd.
##   for Toby Churchill Ltd and Brightstar Engineering
##
## Created by Timothy Manning <timothy@yaffs.net>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License version 2 as
## published by the Free Software Foundation.
##

import os
from yaffsfs import *
import sys
import ctypes


dir_in_snapshot=[]
files_in_snapshot=[]
symlinks_in_snapshot=[]
unknown_in_snapshot=[]
is_mount_in_snapshot=[]
def check_for_yaffs_errors(output):
    if output<0:
        ##error has happened
        error=ctypes.c_int()
        error=yaffs_get_error()
        debug_message("error######################################",0)
        debug_message(("error code", error),  0)
        error_message=ctypes.c_char_p()
        error_message.value=yaffs_error_to_str(error);
        print "error message", error_message.value
        

def debug_message(message, debug_level):
    """note: that debug level 0 will always be printed unless debug_level is set to -1"""
    """level 0 error messages"""
    """level 1 basic tasks are shown(creating, deleating,ect)"""
    """level 2 all process are shown"""
    """level 3 shows minor tasks such as join_paths, ect"""
    """level 4 is used for bug hunting and shows each step in detail"""
    if current_debug_level>=debug_level:
#        for i in range(0, len(message)):
#            print message, 
#        print"\n \n \n"
        print message
def join_paths(path1, path2):
    new_path=path1
    if path1[len(path1)-1]=="/"and path2[0]=="/":
        new_path+=path2[1:]
    elif path1[len(path1)-1]!="/"and path2[0]!="/":
        new_path+="/"
        new_path+=path2
    else:
        new_path+=path2
    
    debug_message(("adding path ", path1, " to ",  path2, " resulting path: ", new_path), 3)
    return new_path
    
def subtract_paths(path1, path2):
    if len(path1)>len(path2):
        if path1[len(path1)-1]!="/":
            path1 +="/"
        if path2[len(path2)-1]!="/":
            path2 += "/"
        debug_message("the two path1 is longer than path2 and can therefore be subtracted.", 4)
        ##if the two paths are diretly subtractable
        if path1[0:len (path2)-1]==path2:
            debug_message("the two paths are direcly subtractable", 4)
            new_path=path1[len(path2):]
        elif path1[1:len (path2)-1]==path2:
            debug_message("the path1 has one more charecter at the begining. assuming that the first chareter is a slash", 4)##fix this assumption.
            new_path=path1[len(path2)+1:]
        elif path1[1:len (path2)]==path2[1:]:
            debug_message("the path2 has one more charecter at the begining. assuming that the first chareter is a slash", 4)##fix this assumption.

            new_path=path1[len(path2)-1:]
        else :
            debug_message("error:could not subtract paths", 0)
            debug_message( ("paths do not match:"+ path1+ "  "+path2), 0)
            return 0
    else :
        debug_message( ("cannot subtract path2(:", path2, ") from path1(", path1, ")because path 2 is too long"), 0)
        
        return 0
    debug_message(("subtracting paths ", path2, " from ",  path1, " resulting path: ", new_path), 3)
    return new_path
    
def create_file(file):
#    freespace=ctypes.c_longlong
#    freespace.value=yaffs_freespace(yaffs_root_dir_path)
#    print "yaffs free space:", freespace.value
    debug_message( "\n \n \n", 2)
    file_path= join_paths(yaffs_root_dir_path, file["path"][len(path):])
    debug_message( ("creating file:", file_path), 2)
    debug_message (("mode", file["mode"]), 2)
    debug_message("opening file",2)
#    yaffs_ls(file["path"])

    ##if there is already a file in yaffs then remove the file . this is to prevent yaffs from opening and writing to a read only file
    if yaffs_access(file_path, 0)==0:##the 0 means does it exist. 
        debug_message ("file already exists in yaffs", 2)
        output=yaffs_unlink(file_path)
        debug_message(("unlinking", file_path, output), 2)
        check_for_yaffs_errors(output)
    
    current_handle=yaffs_open(file_path, yaffs_O_CREAT | yaffs_O_TRUNC| yaffs_O_RDWR, yaffs_S_IREAD | yaffs_S_IWRITE)  ##opens a file with mode set to write
    debug_message(("current_handle", current_handle), 2)
    data_file=open(file["path"], "r")
    output=yaffs_lseek(current_handle, 0, 0)
    if output==-1:
        ##if there is no more space in the emfile then this is where it will show up.
        freespace=ctypes.c_longlong
        freespace.value=yaffs_freespace(yaffs_root_dir_path)
        print "yaffs free space:", freespace.value

        if freespace.value==0:
            #print "yaffs free space:", yaffs_freespace(yaffs_root_dir_path)
            print "yaffs is out of space exiting program"
           #sys.exit() 
        debug_message("error with yaffs lseeking", 2)
        
        check_for_yaffs_errors(output)
    data_to_be_written= data_file.read()
    
    
    length_of_file=len(data_to_be_written)
    debug_message (("length of data to be written",length_of_file), 3) 
    output=yaffs_write(current_handle,data_to_be_written , length_of_file)
    if output>=0:
        debug_message(( "writing to ", file_path," ",  output), 1)
    else :
        debug_message(( "error writing file:", output), 0)
        check_for_yaffs_errors(output)
    output=yaffs_ftruncate(current_handle, length_of_file)
    if output>=0:
        debug_message(( "truncating file:", output), 2)
    else :
        debug_message(( "error truncating file:", output), 0)
        check_for_yaffs_errors(output)
    output=yaffs_close(current_handle)
    if output>=0:
        debug_message(( "closing file:", output), 2)
    else :
        debug_message(( "error closing file:", output), 0)
        check_for_yaffs_errors(output)
    ##changes the mode of the yaffs file to be the same as the scanned file
    yaffs_chmod(file_path, file["mode"]);
    if output>=0:
        debug_message(( "chmoding file:", output), 2)
    else :
        debug_message(( "error chmoding file:", output), 0)
        check_for_yaffs_errors(output)



def create_dir(dir, scanned_path, yaffs_path):
    debug_message( "\n \n \n", 2)
    absolute_dir_path=join_paths(yaffs_path, subtract_paths(dir["path"],scanned_path)) 
    debug_message( ("creating dir:", absolute_dir_path), 2)
    debug_message (("mode(in octal", oct(dir["mode"])), 2)

       ##this is a bug in yaffs which will not make a dir if there is a slash on the end
    if absolute_dir_path[len(absolute_dir_path)-1]=="/":
        absolute_dir_path=absolute_dir_path[0:len(absolute_dir_path)-1]
        debug_message (("path has slash on the end. removing slash new path is:",absolute_dir_path) , 4)
        
        
    ##if there is already a dir in yaffs then remove the dir . this is to clean the yaffs folder if it already exists.
    ##in yaffs all of the files in the dir to be removed must be empty.
    ##need to create a reverse ls to delete all of the files first.
#    if yaffs_access(absolute_dir_path, 0)==0:##the 0 means does it exist. 
#        debug_message ("folder already exists in yaffs", 2)
#        output=yaffs_rmdir(absolute_dir_path)
#        debug_message(("unlinking", absolute_dir_path, output), 2)
#        check_for_yaffs_errors(output)
        
 
        
    output=yaffs_mkdir(absolute_dir_path, dir["mode"] )
    if output>=0:
        debug_message(( "created dir:", absolute_dir_path," ", output), 1)
    else :
        debug_message(( "error creating dir ", absolute_dir_path, " ", output), 0)
        check_for_yaffs_errors(output)
        if output==17:
            printf("the directory already exists")
    
    


def remove_file_from_path(path):
    slash_id=[]
    for i in range(0, len(path)):
        if path[i]=="/":
            slash_id.append(i)
    new_path=path[:slash_id[len(slash_id)-1]]
    debug_message( ("removed file from path", new_path), 2)
    return new_path
    
def is_dir_hidden(dir):
    """this code tests if a directory is hidden (has a ./<name> format) and returns true if it is hidden"""
    slash_id=[]
    for i in range(0, len(dir)):
        if dir[i]=="/":
            slash_id.append(i)

    if dir[slash_id[len(slash_id)-1]+1]==".":
        return True
    else :
        return False
    
def scan_dir(path, search_hidden_directories=True, ):
    """this function scans all of the files and directories in a directory. The function then calls its self on any of the directories that it found. this causes it to build up a tree of all the files and directories """
    global files_in_snapshot
    global symlinks_in_snapshot
    global dir_in_snapshot
    dir_in_current_dir=[]
    global unknown_in_snapshot
#    files_in_snapshot=[]
#    symlinks_in_snapshot=[]
#    dir_in_snapshot=[]
#    dir_in_current_dir=[]
#    unknown_in_snapshot=[]
    if os.path.exists(path)==False:
        debug_message ("error#############################",0)
        debug_message (("path:", path, "  doesnot exist"), 0)
        return 0
    dir_snapshot=os.listdir(path)
    for i in range(0, len(dir_snapshot)):

        current_snapshot=os.path.join(path, dir_snapshot[i])
        ##debug_message (("current snapshot:", current_snapshot), 2)
        isDir=os.path.isdir(current_snapshot)
        isFile=os.path.isfile(current_snapshot)
        isLink=os.path.islink(current_snapshot)
        isMount=os.path.ismount(current_snapshot)

        stat=os.lstat(current_snapshot)
        
        ##note the order of these if and elif statemens is importaint since a file can be symbloic link and a file
        if isDir:
            if search_hidden_directories==True or (is_dir_hidden(current_snapshot) ==False or search_hidden_directories==True ) :
#                st_mode ##mode of the folder read/write ect
                dir_in_snapshot.append({"path":current_snapshot, "mode":stat.st_mode})
                dir_in_current_dir.append(current_snapshot)
            else :
                debug_message( ("file is hidden so it is ingored", current_snapshot,), 1)
        elif  isLink:

            ##for some reason the os.readlink only gives the target link realative to the directory which the symbloic link is in. change this into a absolute path
            x=current_snapshot
            x=remove_file_from_path(x)
            target=join_paths(x,os.readlink(current_snapshot) )
            symlinks_in_snapshot.append({"path":current_snapshot, "target":target})
        elif isFile:

#            stat.st_ino ##inode number
#            st_nlink ##number of hard links to this file
#            st_size ##size of file
            files_in_snapshot.append({"path":current_snapshot, "inode": stat.st_ino, "size":stat.st_size, "num_of_hardlinks":stat.st_nlink, "mode":stat.st_mode})

#        elif isMount:
#            is_mount_in_snapshot.append(current_snapshot)
        else:
            unknown_in_snapshot.append(current_snapshot)
            
    for i in range(0, len(dir_in_current_dir)):
        debug_message(("scanning dir", dir_in_current_dir[i]) , 2)
        scan_dir(dir_in_current_dir[i], search_hidden_directories)
        
#        #debug_message(("data 0", data[0][0]), 2)
#        if len(files)
#        files_in_snapshot.append(data[0][0])
#        dir_in_snapshot.append(data[1][0])
#        symlinks_in_snapshot.append(data[2][0])
#        unknown_in_snapshot.append(data[3][0])
    return (files_in_snapshot, dir_in_snapshot, symlinks_in_snapshot, unknown_in_snapshot)
##
##def print_scanned_dir_list():
##    global files_in_snapshot
##    global symlinks_in_snapshot
##    print( "scanning dir", 2)
##
##
##    for i in range(0, len(files_in_snapshot)):
##        if files_in_snapshot[i]["num_of_hardlinks"]>1:
##            print "inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"], "    num of hard links",  files_in_snapshot[i]["num_of_hardlinks"] 
##
##        else :
##            print "inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"]
###        current_open_file=open(files_in_snapshot[i], "r")
###        #current_open_file.f.read(3)
###        lines_in_file=current_open_file.readlines()
###        #use for loop to write code into yaffs file
###        print "number of line of code:", len(lines_in_file)
###    print current_open_file
##    for i in range(0, len(symlinks_in_snapshot)):
##        print "symlinks in snapshot:", symlinks_in_snapshot[i]
##    for i in range(0, len(dir_in_snapshot)):
##        print "directories in snapshot:", dir_in_snapshot[i]
##    for i in range(0, len(unknown_in_snapshot)):
##        print "unknown objects in snapshot:", unknown_in_snapshot[i]
##


def copy_scanned_files_into_yaffs(files_in_snapshot, dir_in_snapshot,  symlinks_in_snapshot, unknown_in_snapshot,   path, yaffs_root_dir_path="/yaffs2/", yaffs_mount_point_path="/yaffs2/" ):
#files_in_snapshot, dir_in_snapshot, symlinks_in_snapshot, unknown_in_snapshot
#########################################copy directories into yaffs so the files can be created in these directories
    debug_message("making directories in yaffs", 1)
    if yaffs_root_dir_path!=yaffs_mount_point_path:
        slash_id=[]
        debug_message("making directories to the place in yaffs where the directories will copied to", 2)
        root_branch_path=subtract_paths(yaffs_root_dir_path, yaffs_mount_point_path)
        for i in range(0, len(root_branch_path)):

            if root_branch_path[i]=="/" and i != 0:
               slash_id.append(i)
        debug_message(("slash_id", slash_id),4) 
        for i in range(0, len(slash_id)):
            create_dir({"path":root_branch_path[:slash_id[i]], "mode": yaffs_S_IREAD | yaffs_S_IWRITE}, "/", yaffs_mount_point_path) 
    
    for i in range(0, len(dir_in_snapshot)):
        create_dir(dir_in_snapshot[i], path, yaffs_root_dir_path)
  
    
    
#########################################copy file into yaffs
    debug_message("copying scanned files into yaffs", 1)
    list=[]
    inode_blacklist=[]

    debug_message("files to be copyied into yaffs", 2)
    for a in range(0, len(files_in_snapshot)):
        debug_message(files_in_snapshot[a], 2)
    debug_message("\n\n\n", 2)
    for i in range(0, len(files_in_snapshot)):
        list=[]
        if files_in_snapshot[i]["num_of_hardlinks"]>1 and files_in_snapshot[i]["inode"] not in inode_blacklist :
            debug_message("found a hard link", 2)
            debug_message(("inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"], "    num of hard links",  files_in_snapshot[i]["num_of_hardlinks"] ), 2)
            for a in range(0, len(files_in_snapshot) ) :
                if files_in_snapshot[a]["inode"] ==files_in_snapshot[i]["inode"]  :
                    ##and os.path.isfile(files_in_snapshot[i])
                    debug_message(("found this file which matches inode",files_in_snapshot[a]), 2) 
                    list.append(files_in_snapshot[a])
                    debug_message(("length of list", len(list)), 2)
                if len(list)==files_in_snapshot[i]["num_of_hardlinks"]:
                    break
            for a in range(0, len(list)):
                debug_message(list[a], 2)
            ##add inode to blacklist. all of the indoes in the list should be the same.
            inode_blacklist.append(list[0]["inode"])
            ##create a file from the first hardlink.
            create_file(list[0])
            target_path=yaffs_root_dir_path+list[0]["path"][len(path):]
            for i in range(1, len(list)):
                debug_message("creating_symlink", 2)
                debug_message(("target path", target_path), 2)
                hardlink_path=yaffs_root_dir_path+list[i]["path"][len(path):]
                debug_message(("hardlink path", hardlink_path), 2)
                output=yaffs_link(target_path,hardlink_path)
                debug_message(("creating hardlink:", list[i]["path"], "output:", output), 1)
        elif files_in_snapshot[i]["inode"] not in inode_blacklist :
            create_file(files_in_snapshot[i])


############################copy symlinks into yaffs

    for i in range(0, len(symlinks_in_snapshot)):
        debug_message(("symlinks in snapshot:", symlinks_in_snapshot[i]), 2)
        target_path=join_paths(yaffs_root_dir_path, subtract_paths(symlinks_in_snapshot[i]["target"],  path))
        new_path=join_paths(yaffs_root_dir_path, subtract_paths(symlinks_in_snapshot[i]["path"], path))
        output=yaffs_symlink(target_path, new_path)
        debug_message(("created symlink",new_path , " > ", target_path, "  output:", output), 1)
        ##yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath);
   
    
    for i in range(0, len(unknown_in_snapshot)):
        debug_message( ("unknown object in snapshot:", unknown_in_snapshot[i]), 0)
    
    
def import_into_yaffs(file_path, yaffs_path="/yaffs2/", debug_level=1,  copy_hidden_dir=True ,new_yaffs_trace_val=-1 ):
#    global current_debug_level
#    global search_hidden_directories
#    global yaffs_root_dir_path
#    global path
    
#    current_debug_level=debug_level
#    search_hidden_directories=copy_hidden_dir
#    yaffs_root_dir_path=yaffs_path
#    path=file_path
    old_yaffs_trace_val=yaffs_get_trace()
    if new_yaffs_trace_val!=-1:
        yaffs_set_trace(new_yaffs_trace_val)
    
    data=scan_dir(file_path, copy_hidden_dir)
    copy_scanned_files_into_yaffs(data[0], data[1], data[2], data[3],file_path,  yaffs_path)
    
    yaffs_set_trace(old_yaffs_trace_val)
    
    
if __name__=="__main__":
    yaffs_start_up()
    yaffs_mount("/yaffs2/")
    #yaffs_set_trace(0)
#    absolute_path = os.path.abspath(os.path.curdir)
    #print "absolute path:", absolute_path
    current_debug_level=1
    search_hidden_directories=True
    yaffs_root_dir_path="/yaffs2/"
    yaffs_trace=-1
    #print sys.argv
    path=sys.argv[1]
    for i in range(2, len(sys.argv)):
        if sys.argv[i]=="-d":
            current_debug_level=int( sys.argv[i+1])
        if sys.argv[i]=="-ignore_hidden_directories":
            search_hidden_directories=False
        if sys.argv[i]=="-o":
            yaffs_root_dir_path=sys.argv[i+1]
        if sys.argv[i]=="-yaffs_trace":
            yaffs_trace=int(sys.argv[i+1])
#
#
#    path="/home/timothy/work/yaffs/git/yaffs2"
#    path="/home/timothy/my_stuff/old_laptop/timothy/programming_lejos/"


    import_into_yaffs(path, yaffs_root_dir_path, current_debug_level,  search_hidden_directories, yaffs_trace )
#    scan_dir(path)
#    copy_scanned_files_into_yaffs()
    #print_scanned_dir_list()

    print"unmounting yaffs:", yaffs_unmount("/yaffs2/")
