#!/usr/bin/python 
import Tkinter as tk
from yaffsfs import *
#import examples
import ctypes

yaffs_StartUp()
yaffs_mount("yaffs2/")
root_window =tk.Tk()
root_window.title("YAFFS Browser")
mount_list_text_variable=tk.StringVar()

mount_list_text_variable.set("yaffs2/")
current_directory_dict={}
open_windows_list=[]

class editor():
    yaffs_handle=0
    file_editor_root =0
    save_button=[]
    file_contents=0
    file_path=0
    def save_file(self):
        #global current_directory_dict
        print "saving the file"
        print self.file_editor_text.get("1.0", tk.END) ##"1.0" is the index of the first line of text
        yaffs_lseek(self.yaffs_handle, 0, 0)
        data_to_be_written=self.file_editor_text.get("1.0", tk.END)
        print "data to be saved", data_to_be_written
        x=len(data_to_be_written)
        print "length of data to be written",x  
        output=yaffs_write(self.yaffs_handle,data_to_be_written , x)
        print "output", output
        yaffs_close(self.yaffs_handle)
        self.yaffs_handle = yaffs_open(self.file_path,66,0666)
        
    def __init__(self):
        global current_directory_dict
        x=name_list_box.curselection()
        self.id=int(x[0])
        self.file_editor_root =tk.Toplevel()
        self.save_button=tk.Button(self.file_editor_root, text="save", command=self.save_file)
        self.save_button.pack()

        self.file_path=current_directory_dict[self.id]["path"]
        print "file path", self.file_path
        self.file_editor_root.title(current_directory_dict[self.id]["path"])
        self.file_editor_text=tk.Text(self.file_editor_root)
        self.yaffs_handle = yaffs_open(current_directory_dict[self.id]["path"],66,0666)
        length_of_file=yaffs_lseek(self.yaffs_handle, 0, 2) ##seeks to the end of the file
        yaffs_lseek(self.yaffs_handle, 0, 0)## returns the handle to the front of th file
        self.file_contents=ctypes.create_string_buffer(length_of_file)
        yaffs_read(self.yaffs_handle,self.file_contents,length_of_file)
        print "file contents", self.file_contents.raw
        self.file_editor_text.insert(tk.END, self.file_contents.raw)
        self.file_editor_text.pack()


def load_dir():
    global current_directory_dict
    print "loading a new directory*******************************************************************"
    name_list_box.delete(0, tk.END)
    current_directory_dict=yaffs_ls(mount_list_text_variable.get())
    print "new directory", current_directory_dict
    ##copy directory into file box
    for x in range(0,len(current_directory_dict)):
        name_list_box.insert(x,(current_directory_dict[x]["inodes"]+"  "+ current_directory_dict[x]["type"]+"  "+ current_directory_dict[x]["size"]+"  "+ current_directory_dict[x]["path"]))
    name_list_box.grid(column=0, row=1)
    return current_directory_dict
def save_file():
    print file_editor_text.get(0, end)
    pass
def load_file():
    global open_windows_list
    open_windows_list.append(editor())
    


    

   # file_editor_text.insert(0, "this is a text box\n")
    #use file_editor_text_box.get(0,  END) to retreve text
    
def load_command(self=0):
    global current_directory_dict
    print "you loaded a file/dir"
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
        current_directory_dict=load_dir()
        print "new directory dict passed back"

    elif current_directory_dict[x]["type"]=="file" :
        ##open file
        print "open file"
        load_file()
    

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
            result = yaffs_stat(fullname,byref(st))
            perms = st.st_mode & 0777
            isFile = True if st.st_mode & 0x8000 else False
            isDir  = True if st.st_mode & 0x4000 else False

            if isFile :
                ls_dict.append ({"type" :"file",  "inodes" :  str(se.d_ino),   "permissions" : str(hex(perms)),  "size": str(st.st_size), "path":  fullname})
            elif isDir :
                ls_dict.append({"type":"dir", "inodes" :str(se.d_ino), "permissions":str( hex(perms)),"size":"0",   "path": fullname+"/"})

#            else :
#            	ls_dict.append( "Other ("+hex(st.st_mode)+") "+se.d_ino, hex(perms)+ fullname)
            sep = yaffs_readdir(dc)
        yaffs_closedir(dc)
        return ls_dict
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



##mount list entry box init
mount_list_frame=tk.Frame(root_window)
mount_list_label=tk.Label(mount_list_frame, text="mount list")
mount_list_label.pack(side=tk.RIGHT)

mount_list_entry_box= tk.Entry(mount_list_frame,textvariable=mount_list_text_variable)
mount_list_entry_box.pack(side=tk.RIGHT)
mount_list_frame.grid(row=1, column=0, columnspan=2)




list_frame=tk.Frame(root_window)



name_list_box=tk.Listbox(list_frame,exportselection=0, height=20, width=30)

load_dir()

#print "current_dir_dict", current_directory_dict


list_frame.grid()

name_list_box.bind("<Double-Button-1>", load_command)




root_window.mainloop()
yaffs_unmount("yaffs2/")

print"saving"

