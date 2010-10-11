import os
from yaffsfs import *

yaffs_StartUp()
yaffs_mount("/yaffs2/")
absolute_path = os.path.abspath(os.path.curdir)
print "absolute path:", absolute_path
#cwd = os.getcwd()
#print cwd
dir_in_snapshot=[]
files_in_snapshot=[]
symlinks_in_snapshot=[]
unknown_in_snapshot=[]
is_mount_in_snapshot=[]
#def scandir(path):
#    global dir_in_snapshot
#    global files_in_snapshot
#    global symlinks_in_snapshot
#    global unknown_in_snapshot
#path=raw_input("path")
#print "does this path exist?", os.path.exists(path)
#dir_snapshot=os.listdir(path)
#print  dir_snapshot
def is_dir_hidden(dir):
    slash_id=[]
    for i in range(0, len(dir)):
        if dir[i]=="/":
            slash_id.append(i)
    #print "dir", dir
    #print "slash_id", slash_id
    if dir[slash_id[len(slash_id)-1]+1]==".":
        return True
    else :
        return False
        
def scan_dir(path):
    global files_in_snapshot
    global symlinks_in_snapshot
    global search_hidden_directories
    dir_in_current_dir=[]
    if os.path.exists(path)==False:
        print "error#############################"
        print "path:", path, "  doesnot exist"
        return 0
    dir_snapshot=os.listdir(path)
    for i in range(0, len(dir_snapshot)):
        #print os.path.isdir(dir_snapshot[i])
        #print os.path.isfile(dir_snapshot[i])
        #print os.path.islink(dir_snapshot[i])
        current_snapshot=os.path.join(path, dir_snapshot[i])
        #print "current snapshot:", current_snapshot
        isDir=os.path.isdir(current_snapshot)
        isFile=os.path.isfile(current_snapshot)
        isLink=os.path.islink(current_snapshot)
        isMount=os.path.ismount(current_snapshot)
        #print dir_snapshot[i]
        if isDir:
            if search_hidden_directories==True or is_dir_hidden(current_snapshot) ==False :
                stat=os.stat(current_snapshot)
#                st_mode ##mode of the folder read/write ect
                dir_in_snapshot.append({"path":current_snapshot, "mode":stat.st_mode})
                dir_in_current_dir.append(current_snapshot)
            else :
                print "file is hidden so it is ingored", current_snapshot
        elif isFile:
            stat=os.stat(current_snapshot)
#            stat.st_ino ##inode number
#            st_nlink ##number of hard links to this file
#            st_size ##size of file
            files_in_snapshot.append({"path":current_snapshot, "inode": stat.st_ino, "size":stat.st_size, "num_of_hardlinks":stat.st_nlink, "mode":stat.st_mode})
        elif  isLink:
            symlinks_in_snapshot.append(current_snapshot)
        elif isMount:
            is_mount_in_snapshot.append(current_snapshot)
        else:
            unknown_in_snapshot.append(current_snapshot)
            
    for i in range(0, len(dir_in_current_dir)):
        scan_dir(dir_in_current_dir[i])

def print_scanned_dir_list():
    global files_in_snapshot
    global symlinks_in_snapshot
    print "scanned dir"


    for i in range(0, len(files_in_snapshot)):
        if files_in_snapshot[i]["num_of_hardlinks"]>1:
            print "inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"], "    num of hard links",  files_in_snapshot[i]["num_of_hardlinks"] 

        else :
            print "inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"]
#        current_open_file=open(files_in_snapshot[i], "r")
#        #current_open_file.f.read(3)
#        lines_in_file=current_open_file.readlines()
#        #use for loop to write code into yaffs file
#        print "number of line of code:", len(lines_in_file)
#    print current_open_file
    for i in range(0, len(symlinks_in_snapshot)):
        print "symlinks in snapshot:", symlinks_in_snapshot[i]
    for i in range(0, len(dir_in_snapshot)):
        print "directories in snapshot:", dir_in_snapshot[i]
    for i in range(0, len(unknown_in_snapshot)):
        print "unknown objects in snapshot:", unknown_in_snapshot[i]

def copy_scanned_files_into_yaffs():
    global files_in_snapshot
    global symlinks_in_snapshot
    global path
    print"making directories in yaffs"

    for i in range(0, len(dir_in_snapshot)):
        path2=dir_in_snapshot[i]["path"][len(path):]
        print"path2", path2
        dir_path="/yaffs2"+path2
        output=yaffs_mkdir(dir_path,dir_in_snapshot[i]["mode"] )
        print"copied", dir_path,   "  output", output
        print "mode" ,dir_in_snapshot[i]["mode"]
    print "copying scanned files into yaffs"


    for i in range(0, len(files_in_snapshot)):
        if files_in_snapshot[i]["num_of_hardlinks"]>1:
            print "inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"], "    num of hard links",  files_in_snapshot[i]["num_of_hardlinks"] 

        else :
            print "\n \n \n"
            file_path="/yaffs2/"+files_in_snapshot[i]["path"][len(path):]
            print "creating file:", file_path
            print "mode", files_in_snapshot[i]["mode"]
            current_handle=yaffs_open(file_path, yaffs_O_CREAT | yaffs_O_TRUNC| yaffs_O_RDWR, files_in_snapshot[i]["mode"]) 
            data_file=open(files_in_snapshot[i]["path"], "r")
            yaffs_lseek(current_handle, 0, 0)
            data_to_be_written= data_file.read()
            
            
            #print "data to be saved", data_to_be_written
            length_of_file=len(data_to_be_written)
            print "length of data to be written",length_of_file 
            output=yaffs_write(current_handle,data_to_be_written , length_of_file)
            print "writing file:", output
            yaffs_ftruncate(current_handle, length_of_file)
            
            output=yaffs_close(current_handle)
            print "created a file", files_in_snapshot[i]["path"][len(path):], "  ", output
            
            if output==-1:
                print "ran out of space exiting"
                return 0
            #print "inode",files_in_snapshot[i]["inode"],"size",files_in_snapshot[i]["size"],"path:", files_in_snapshot[i]["path"]

#        current_open_file=open(files_in_snapshot[i], "r")
#        #current_open_file.f.read(3)
#        lines_in_file=current_open_file.readlines()
#        #use for loop to write code into yaffs file
#        print "number of line of code:", len(lines_in_file)
#    print current_open_file
    for i in range(0, len(symlinks_in_snapshot)):
        print "symlinks in snapshot:", symlinks_in_snapshot[i]
    for i in range(0, len(dir_in_snapshot)):
        print "directories in snapshot:", dir_in_snapshot[i]
    for i in range(0, len(unknown_in_snapshot)):
        print "unknown objects in snapshot:", unknown_in_snapshot[i]

if __name__=="__main__":
        
    #path=raw_input("path")
    path="/home/timothy/work/yaffs/git/yaffs2"
    path="/home/timothy/my_stuff/old_laptop/timothy/programming_lejos/"

    #path="/home/timothy"

    #x=raw_input("search hidden files and directories?[y/n]")
    x="n"
    if x=="y" or x=="yes":
        search_hidden_directories=True
    else :
        search_hidden_directories=False
    scan_dir(path)
    copy_scanned_files_into_yaffs()
    #print_scanned_dir_list()

    print"unmounting yaffs:", yaffs_unmount("yaffs2/")
