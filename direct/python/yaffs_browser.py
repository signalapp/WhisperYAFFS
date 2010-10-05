import Tkinter as tk
from yaffsfs import *

yaffs_StartUp()
yaffs_mount("yaffs2/")
root_window =tk.Tk()

def load_file(self=0):
    print "youloaded a file"
def back_a_directory(self=0):
    pass
    
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

    

##toolbar 
toolbar_frame=tk.Frame(root_window)
button_open=tk.Button(toolbar_frame, command=load_file, text="load")
button_open.grid(column=0, row=0)
button_back=tk.Button(toolbar_frame, command=back_a_directory, text="back")
button_back.grid(column=1, row=0)
toolbar_frame.grid(row=0, column=0,  columnspan=3)
#root_window.bind("<Double-Button-1>", button_open)



##mount list entry box init
mount_list_frame=tk.Frame(root_window)
mount_list_label=tk.Label(mount_list_frame, text="mount list")
mount_list_label.pack(side=tk.RIGHT)
mount_list_text_variable=tk.StringVar()

mount_list_text_variable.set("path")
mount_list_entry_box= tk.Entry(mount_list_frame,textvariable=mount_list_text_variable)
mount_list_entry_box.pack(side=tk.RIGHT)
mount_list_frame.grid(row=1, column=0, columnspan=2)




list_frame=tk.Frame(root_window)

###header part of the browser
#
#name_button=tk.Button(list_frame, text="name")
#name_button.grid(row=0, column=0)
#
#type_button=tk.Button(list_frame, text="file type")
#type_button.grid(row=0, column=1)
#
#size_button=tk.Button(list_frame, text="size")
#size_button.grid(row=0, column=2)





##list part of the browser
#
#name_list_box=tk.Listbox(list_frame,exportselection=0, height=20, width=30)
#
#
#
#displayed_list=[]
#for x in range(0,list.length_of_list ):
#
#    
#    type_spaces=" "
#    type_spaces=type_spaces* (10- len(list.type[x])) 
#    y=(list.name[x]+name_spaces+list.type[x]+list.size[x])
#    print (list.name[x]+name_spaces+list.type[x]+list.size[x])
#    name_list_box.insert(x,y)
#name_list_box.grid(column=0, row=1)
#
#
#list_frame.grid()
##browser_frame.grid(row=1, column=0,columnspan=3 )
#name_list_box.bind("<Double-Button-1>", load_file)


yaffs_ls("/yaffs2")

root_window.mainloop()
