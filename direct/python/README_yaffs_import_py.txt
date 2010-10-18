Readme for the yaffs_import.py file

python yaffs_import [scanning_path] [optional flags]

the scanning path needs to be the absolute path to the directory where you
wish to start scanning from. this function will scan all of the the
directories above this path and copy the all files in these directories.

example:
	$  yaffs_importer.py /home/timothy/work/yaffs/


flags:
	-d [number] 
		set the debugging message level.
		that debug level 0 will always be printed unless debug_level is set to -1
    		level 0 error messages
		level 1 basic tasks are shown(creating, deleating,ect)(this is set as the
		default and is recommended)
		level 2 all process are shown
		level 3 shows minor tasks such as join_paths, ect
		level 4 is used for bug hunting and shows each step and in detail 
	-ignore_hidden_directories
		will not copy hidden (./) directories if used.

Deleating files and folders
	to deleate files and folders in yaffs use the yaffs_browser (the documentation still needs to be written).
	to run the browser use this: "python yaffs_browser"
	to deleate a file or a folder select the file or folder then go
	Edit->Delete selected.

Clear Yaffs
	the easiest way to clear yaffs of all files and folders is to remove
	the emfile-2k-0 file. this file is stored in the
	yaffs2/direct/python/ folder. 
	the command is: rm emfile-2k-0  