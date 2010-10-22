Readme for the yaffs_import.py file
by Timothy Manning <timothy@yaffs.net> 2010

python yaffs_import [scanning_path] [optional flags]

the scanning path needs to be the absolute path to the directory where you
wish to start scanning from. this function will scan all of the the
directories above this path and copy the all files in these directories.

example:
	$  yaffs_importer.py /home/timothy/work/yaffs/

yaffs_import.py must be run in yaffs/direct/python/ directory.

flags:
	-d [number] 
		set the debugging message level.
    		level 0 error messages
		level 1 basic tasks are shown(creating, deleating,ect)(this is set as the
		default and is recommended)
		level 2 all process are shown
		level 3 shows minor tasks such as join_paths, ect
		level 4 is used for bug hunting and shows each step and in detail 
	-ignore_hidden_directories
		will not copy hidden (./) directories if used.
	
	-o [yaffs_path]
		chooses the path where the branch will be copyied into yaffs.
		note this path must start with "/yaffs2/" 
		example: python yaffs_importer.py /home/timothy/work/yaffs/yaffs_importer_test_dir -o /yaffs2/apple/

	-yaffs_trace [number]
		this sets the yaffs_trace() function with the number inputed.
		this number must be in the range of char.
		yaffs_importer saves the current yaffs_trace value and restores the value after the branch has been imported.
		setting this flag to -1 will run yaffs_importer with the default yaffs_trace value.  

Deleting files and folders
	to deleate files and folders in yaffs use the yaffs_browser (the documentation still needs to be written).
	to run the browser use this: "python yaffs_browser"
	to deleate a file or a folder select the file or folder then go
	Edit->Delete selected.

Clear Yaffs
	the easiest way to clear the yaffs file system of all files and folders is to remove
	the emfile-2k-0 file. this file is stored in the
	yaffs2/direct/python/ folder. 
	the command is: rm emfile-2k-0  

TODO list 
	Add fix the problem of yaffs_write() returning -1 when the disk is full.(generate a error message)
	Add a yaffs error code refrence (prints a text message about the error)  

