#!/usr/bin/python 
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

import Tkinter as tk
from yaffsfs import *
#import examples
import ctypes
from operator import itemgetter

yaffs_start_up()
yaffs_mount("/yaffs2/")
root_window =tk.Tk()
root_window.title("YAFFS Browser")
mount_list_text_variable=tk.StringVar()

mount_list_text_variable.set("/yaffs2/")
current_directory_dict={}
open_windows_list=[]

class editor():
    yaffs_handle=0
    file_editor_root =0
    save_button=[]
    file_contents=0
    file_path=0
    isLink=0
    def save_file(self):
        if  self.isLink==True:
            target_path=self.file_editor_text.get("1.0", tk.END) ##"1.0" is the index of the first line of text
            target_path=target_path[0:len(target_path)-1]
            new_path=self.file_path
            print "creating a symlink \n target:##", target_path, "##"
            target_file_exists=yaffs_access(target_path, 0) ##yaffs_access will return 0 on success and -1 on failure.
            if target_file_exists>=0:
                ##file exists,create symlink
                print "target file exist, creating symlink"
                yaffs_unlink(new_path)
                output=yaffs_symlink(target_path, new_path)
                print "yaffs symlink output=", output
                self.file_editor_root.destroy()
            else :
                ##file does not exist
                print "target file does not exist, cannot create symlink"
        else :
            #global current_directory_dict
            print "saving the file"
            print self.file_editor_text.get("1.0", tk.END) ##"1.0" is the index of the first line of text
            yaffs_lseek(self.yaffs_handle, 0, 0)
            data_to_be_written=self.file_editor_text.get("1.0", tk.END)
            print "data to be saved", data_to_be_written
            length_of_file=len(data_to_be_written)
            print "length of data to be written",length_of_file 
            output=yaffs_write(self.yaffs_handle,data_to_be_written , length_of_file)
            print "output", output
            yaffs_ftruncate(self.yaffs_handle, length_of_file)
            yaffs_close(self.yaffs_handle)
            self.yaffs_handle = yaffs_open(self.file_path,66,0666)
        load_dir()
        
    def __init__(self, isLink=0):
        global current_directory_dict
        x=name_list_box.curselection()
        self.id=int(x[0])
        self.file_editor_root =tk.Toplevel()
        self.save_button=tk.Button(self.file_editor_root, text="save", command=self.save_file)
        self.save_button.pack(fill=tk.BOTH)

        self.file_path=current_directory_dict[self.id]["path"]
        print "file path", self.file_path
        self.file_editor_root.title(current_directory_dict[self.id]["path"])
        self.file_editor_text=tk.Text(self.file_editor_root)
        self.yaffs_handle = yaffs_open(current_directory_dict[self.id]["path"],66,0666)
        length_of_file=yaffs_lseek(self.yaffs_handle, 0, 2) ##seeks to the end of the file
        yaffs_lseek(self.yaffs_handle, 0, 0)## returns the handle to the front of th file
        print "length of file to be opened:", length_of_file
        if isLink==True and False ==True : ##this alows the symlink to be edited and is no longer used. to renable it delete "and False ==True"
            print "opening symlink"
            self.file_contents=ctypes.create_string_buffer(1000)
            yaffs_readlink(self.file_path,self.file_contents,1000)
            self.isLink=True
        else:
            print"opening file"
            self.file_contents=ctypes.create_string_buffer(length_of_file)
            yaffs_read(self.yaffs_handle,self.file_contents,length_of_file)
            print "file contents", self.file_contents.raw
        self.file_editor_text.insert(tk.END, self.file_contents.raw)
        self.file_editor_text.pack(fill=tk.BOTH)
        ##self.file_editor_text.bind("<Control-s>", self.save_file)
        ##doesn't work because it can't pass "self"

def load_dir():
    global current_directory_dict
    print "loading a new directory*******************************************************************"
    ##deleate current items in text box
    name_list_box.delete(0, tk.END)
    current_directory_dict=yaffs_ls(mount_list_text_variable.get())
    print "new directory", current_directory_dict
    
   
    
    ##copy directory into file box
    for x in range(0,len(current_directory_dict)):
        permissions_string=""
        if current_directory_dict[x]["permissions"] & yaffs_S_IREAD:
            permissions_string ='r'
        else :
            permissions_string ='-'
        if current_directory_dict[x]["permissions"] & yaffs_S_IWRITE:
            permissions_string+='w'
        else :
            permissions_string+='-'
        name_list_box.insert(x,(current_directory_dict[x]["inodes"]+"  "+permissions_string+"  "+ current_directory_dict[x]["type"]+"  "+ current_directory_dict[x]["size"]+"  "+ current_directory_dict[x]["path"]+"  "+current_directory_dict[x]["extra_data"]))
    name_list_box.grid(column=0, row=1)
    return current_directory_dict
    
def remount_yaffs():
    ##this isn't working. somethihg need to be changed in the config of the simulator to release the handle of the emfile
    print "remounting yaffs"
    print"unmounting yaffs:", yaffs_unmount("yaffs2/")
    print "mounting yaffs", yaffs_mount("/yaffs2/")
    load_dir()
    
def load_file(link=0):
    global open_windows_list
    open_windows_list.append(editor(link))

def load_command(self=0):
    global current_directory_dictls
    
    print "you loaded a file/dir/link"
    x=name_list_box.curselection()
    x=int(x[0])
    print "cursor selection", x
    print "dict", current_directory_dict

    print "file inode is:", current_directory_dict[x]["inodes"]
    
    
    print "file path is:", current_directory_dict[x]["path"]
    if current_directory_dict[x]["type"]=="dir":
        ##open dir
        print "open directory"
        mount_list_text_variable.set(current_directory_dict[x]["path"])
        print mount_list_text_variable.get()
        print "old directory dict", current_directory_dict
        #current_directory_dict=load_dir()
        load_dir()

        print "new directory dict passed back"

    elif current_directory_dict[x]["type"]=="file" :
        ##open file
        print "open file"
        load_file()
    elif current_directory_dict[x]["type"]=="link": 
        print "loading a symlink"
        load_file(1)
    ##mount_list_text_variable.set(mount_list_text_variable.get()+str(list[0][0]))
    
def back_a_directory(self=0):
    x=len(mount_list_text_variable.get())
    string=mount_list_text_variable.get()
    slashes_id=[]
    #print "length of path", x 
    #print "string been sorted:",  string
    for i in range(0, x):
        if string[i]=='/':
            slashes_id.append(i)
            #print "slash found at:", i
        
    print slashes_id
    ##slashes_id.sort() not needed because the list is already in acending order
    ##print "sorted",slashes_id

    string=string[0: slashes_id[len(slashes_id)-2]+1]
    print string
    mount_list_text_variable.set(string)
    load_dir()


    
    
    
 
def yaffs_ls(dname):
    ls_dict=[]

    if dname[-1] != "/": dname = dname + "/"
    dc = yaffs_opendir(dname)
    if dc != 0 :
        sep = yaffs_readdir(dc)
        while bool(sep):
            
            se = sep.contents
            fullname = dname + se.d_name
            st = yaffs_stat_struct()
            result = yaffs_lstat(fullname,byref(st))
            #perms = st.st_mode & 0777
            perms = st.st_mode 
            ftype = st.st_mode & yaffs_S_IFMT
            isFile = True if ftype == yaffs_S_IFREG else False
            isDir  = True if ftype == yaffs_S_IFDIR else False
            isSymlink= True if ftype == yaffs_S_IFLNK else False

            if isFile :
                ls_dict.append ({"type" :"file",  "inodes" :  str(se.d_ino),   "permissions" : perms,  "size": str(st.st_size), "path":  fullname,"extra_data":""})
                print "file st.st_mode:", st.st_mode

            elif isDir :
                print "dir st.st_mode:", st.st_mode

                ls_dict.append({"type":"dir", "inodes" :str(se.d_ino), "permissions":perms,"size":"0",   "path": fullname+"/", "extra_data":""})
            elif isSymlink:
                print "symlink st.st_mode:", st.st_mode
                file_contents=ctypes.create_string_buffer(30)
                yaffs_readlink(fullname,file_contents,30)
                string=repr(file_contents.value)
                print "len of str", len(string)
#                string.lstrip()

                print "string", string, "###"

                ls_dict.append ({"type" :"link",  "inodes" :  str(se.d_ino),   "permissions" : perms,  "size": str(st.st_size), "path":  fullname, "extra_data":"> "+string})

            else :
                print "unknown st.st_mode:", st.st_mode
                ls_dict.append({ "type":"Other", "inodes":str(se.d_ino),  "permissions":perms, "size":"0",   "path": fullname,"extra_data":""})
            sep = yaffs_readdir(dc)
        yaffs_closedir(dc)
        return sorted(ls_dict,key=itemgetter("path"))
    else:
        print "Could not open directory"
        return -1


##toolbar 
toolbar_frame=tk.Frame(root_window)
button_open=tk.Button(toolbar_frame, command=load_command, text="load")
button_open.grid(column=0, row=0)
button_back=tk.Button(toolbar_frame, command=back_a_directory, text="back")
button_back.grid(column=1, row=0)
toolbar_frame.grid(row=0, column=0,  columnspan=3)

def delete_selected(selected_dir=0):
    if selected_dir==0:
        print"using current_directory_dict"
        global current_directory_dict
        x=name_list_box.curselection()
        x=int(x[0])
        print current_directory_dict[x]["type"]
        if current_directory_dict[x]["type"]=="file":
            path=current_directory_dict[x]["path"]
            path =path
            output=yaffs_unlink(path)
            print "unlinking output:", output
        elif current_directory_dict[x]["type"]=="dir":
            path=current_directory_dict[x]["path"]
            inside_dir=yaffs_ls(path)
            print "files and folder inside dir", inside_dir
            print "len of dir", len(inside_dir)
            if inside_dir!=[]: ##if the dir is not empty
                ## remove stuff in dir
                for i in range(0,len(inside_dir)):
                    print "calling self*****"
                    delete_selected(inside_dir[i])
                
            path =path[0:len(path)-1] ##this is to remove the "/" off the end of the of the file 
            print "removing:", path
            output=yaffs_rmdir(path)
            print "rmdir output:", output
    else :
        print "using passed dir"
        print "dir passed", selected_dir
        current_directory_dict =selected_dir

        print "after copying", current_directory_dict
        print current_directory_dict["type"]
        if current_directory_dict["type"]=="file":
            path=current_directory_dict["path"]
            path =path
            output=yaffs_unlink(path)
            print "unlinking output:", output
        elif current_directory_dict["type"]=="dir":
            path=current_directory_dict["path"]
            inside_dir=yaffs_ls(path)
            print "files and folder inside dir", inside_dir
            print "len of dir", len(inside_dir)
            if inside_dir!=[]: ##if the dir is not empty
                ## remove stuff in dir
                for i in range(0,len(inside_dir)):
                    print "calling self*****"
                    delete_selected(inside_dir[i])
                
            path =path[0:len(path)-1] ##this is to remove the "/" off the end of the of the file 
            print "removing:", path
            output=yaffs_rmdir(path)
            print "rmdir output:", output
            
    load_dir()



class new_file():
    path_entry_box=0
    new_file_window=0
    def open_the_file(self):
        global mount_list_text_variable
        print "trying to create", mount_list_text_variable.get()+self.path_entry_box.get()
        yaffs_handle=yaffs_open(self.path_entry_box.get(),66,0666)
        yaffs_close(yaffs_handle)
        self.new_file_window.destroy()
        load_dir()

    def cancel(self):
        ##del self
        self.new_file_window.destroy()
    def __init__(self):
        global mount_list_text_variable
        self.new_file_window =tk.Toplevel(takefocus=True)
        path_frame=tk.Frame(self.new_file_window)
        path_label=tk.Label(path_frame, text="file path")
        path_label.pack(side=tk.LEFT)
        text=tk.StringVar()
        text.set(mount_list_text_variable.get())
        print "############################",mount_list_text_variable.get()
        self.path_entry_box= tk.Entry(path_frame, textvariable=text)
        self.path_entry_box.pack(side=tk.RIGHT)
        path_frame.pack()
        button_frame=tk.Frame(self.new_file_window)
        create_button=tk.Button(button_frame, text="Create", command=self.open_the_file)
        create_button.pack(side=tk.LEFT)
        cancel_button=tk.Button(button_frame, text="Cancel", command=self.cancel)
        cancel_button.pack(side=tk.RIGHT)
        button_frame.pack()

class new_folder():
    path_entry_box=0
    new_folder_window=0
    def create_the_folder(self):
        global mount_list_text_variable
        print "trying to create", mount_list_text_variable.get()+self.path_entry_box.get()
        yaffs_mkdir(self.path_entry_box.get(),0666)
        self.new_folder_window.destroy()

        load_dir()

    def cancel(self):
        ##del self
        self.new_folder_window.destroy()
    def __init__(self):
        global mount_list_text_variable
        self.new_folder_window =tk.Toplevel(takefocus=True)
        path_frame=tk.Frame(self.new_folder_window)
        path_label=tk.Label(path_frame, text="directory path")
        path_label.pack(side=tk.LEFT)
        text=tk.StringVar()
        text.set(mount_list_text_variable.get())
        print "############################",mount_list_text_variable.get()
        self.path_entry_box= tk.Entry(path_frame, textvariable=text)
        self.path_entry_box.pack(side=tk.RIGHT)
        path_frame.pack()
        button_frame=tk.Frame(self.new_folder_window)
        create_button=tk.Button(button_frame, text="Create", command=self.create_the_folder)
        create_button.pack(side=tk.LEFT)
        cancel_button=tk.Button(button_frame, text="Cancel", command=self.cancel)
        cancel_button.pack(side=tk.RIGHT)
        button_frame.pack()


class new_symlink():
    path_entry_box=0
    target_text=0
    new_text=0
    new_file_window=0
    def create_the_symlink(self):
        global mount_list_text_variable
        ##check the symlink's target is a file.
        target_path=self.target_text.get()
        new_path=self.new_text.get()
        print "creating a symlink \n target:", target_path
        target_file_exists=yaffs_access(target_path, 0) ##yaffs_access will return 0 on success and -1 on failure.
        if target_file_exists>=0:
            ##file exists,create symlink
            print "target file exist, creating symlink"
            yaffs_symlink(target_path, new_path)
            self.new_file_window.destroy()
        else :
            ##file does not exist
            print "target file does not exist, cannot create symlink"
        load_dir()

    def cancel(self):
        ##del self
        self.new_file_window.destroy()
    def __init__(self):
        global mount_list_text_variable
        self.new_file_window =tk.Toplevel(takefocus=True)
        target_frame=tk.Frame(self.new_file_window)
        target_label=tk.Label(target_frame, text="target")
        target_label.pack(side=tk.LEFT)
        self.target_text=tk.StringVar()
        self.target_text.set(mount_list_text_variable.get())
        #print "############################",mount_list_text_variable.get()
        self.target_entry_box= tk.Entry(target_frame, textvariable=self.target_text)
        self.target_entry_box.pack(side=tk.RIGHT)
        target_frame.pack()
        
        new_frame=tk.Frame(self.new_file_window)
        new_label=tk.Label(new_frame, text="file path")
        new_label.pack(side=tk.LEFT)
        self.new_text=tk.StringVar()
        self.new_text.set(mount_list_text_variable.get())
        #print "############################",mount_list_text_variable.get()
        self.new_entry_box= tk.Entry(new_frame, textvariable=self.new_text)
        self.new_entry_box.pack(side=tk.RIGHT)
        new_frame.pack()
        
        button_frame=tk.Frame(self.new_file_window)
        create_button=tk.Button(button_frame, text="Create", command=self.create_the_symlink)
        create_button.pack(side=tk.LEFT)
        cancel_button=tk.Button(button_frame, text="Cancel", command=self.cancel)
        cancel_button.pack(side=tk.RIGHT)
        button_frame.pack()



class new_hardlink():
    path_entry_box=0
    target_text=0
    new_text=0
    new_file_window=0
    def create_the_hardlink(self):
        global mount_list_text_variable
        ##check the symlink's target is a file.
        target_path=self.target_text.get()
        new_path=self.new_text.get()
        print "creating a hardlink \n target:", target_path
        target_file_exists=yaffs_access(target_path, 0) ##yaffs_access will return 0 on success and -1 on failure.
        if target_file_exists>=0:
            ##file exists,create symlink
            print "target file exist, creating hardlink"
            yaffs_link(target_path, new_path)
            self.new_file_window.destroy()
        else :
            ##file does not exist
            print "target file does not exist, cannot create hardlink"
        load_dir()

    def cancel(self):
        ##del self
        self.new_file_window.destroy()
    def __init__(self):
        global mount_list_text_variable
        self.new_file_window =tk.Toplevel(takefocus=True)
        target_frame=tk.Frame(self.new_file_window)
        target_label=tk.Label(target_frame, text="target")
        target_label.pack(side=tk.LEFT)
        self.target_text=tk.StringVar()
        self.target_text.set(mount_list_text_variable.get())
        #print "############################",mount_list_text_variable.get()
        self.target_entry_box= tk.Entry(target_frame, textvariable=self.target_text)
        self.target_entry_box.pack(side=tk.RIGHT)
        target_frame.pack()
        
        new_frame=tk.Frame(self.new_file_window)
        new_label=tk.Label(new_frame, text="file path")
        new_label.pack(side=tk.LEFT)
        self.new_text=tk.StringVar()
        self.new_text.set(mount_list_text_variable.get())
        #print "############################",mount_list_text_variable.get()
        self.new_entry_box= tk.Entry(new_frame, textvariable=self.new_text)
        self.new_entry_box.pack(side=tk.RIGHT)
        new_frame.pack()
        
        button_frame=tk.Frame(self.new_file_window)
        create_button=tk.Button(button_frame, text="Create", command=self.create_the_hardlink)
        create_button.pack(side=tk.LEFT)
        cancel_button=tk.Button(button_frame, text="Cancel", command=self.cancel)
        cancel_button.pack(side=tk.RIGHT)
        button_frame.pack()


##mount list entry box init
mount_list_frame=tk.Frame(root_window)
mount_list_label=tk.Label(mount_list_frame, text="mount list")
mount_list_label.pack(side=tk.RIGHT)

mount_list_entry_box= tk.Entry(mount_list_frame,textvariable=mount_list_text_variable)
mount_list_entry_box.pack(side=tk.RIGHT)
mount_list_frame.grid(row=1, column=0, columnspan=2)


list_frame=tk.Frame(root_window)
name_list_box=tk.Listbox(list_frame,exportselection=0, height=30, width=80)
load_dir()
list_frame.grid(sticky=tk.W+tk.E+tk.N+tk.S)

name_list_box.bind("<Double-Button-1>", load_command)

browser_menu_bar=tk.Menu(root_window)
browser_file_menu=tk.Menu(browser_menu_bar)

browser_file_menu.add_command(label="Reload", command=load_dir)
browser_file_menu.add_command(label="Remount yaffs", command=remount_yaffs)

#browser_file_menu.add_command(label="Open")
#browser_file_menu.add_command(label="Save")
browser_menu_bar.add_cascade(label="File", menu=browser_file_menu)
root_window.config(menu=browser_menu_bar)


browser_edit_menu=tk.Menu(browser_menu_bar)

browser_edit_menu.add_command(label="New File", command=new_file)
browser_edit_menu.add_command(label="New Folder", command=new_folder)
browser_edit_menu.add_command(label="New Symlink", command=new_symlink)
browser_edit_menu.add_command(label="New Hardlink", command=new_hardlink)


browser_edit_menu.add_command(label="delete selected", command=delete_selected)
browser_menu_bar.add_cascade(label="Edit", menu=browser_edit_menu)






root_window.mainloop()

print"unmounting yaffs:", yaffs_unmount("yaffs2/")


