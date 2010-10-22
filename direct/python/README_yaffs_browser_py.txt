Readme for the yaffs_browser.py file
by Timothy Manning <timothy@yaffs.net> 2010

to run yaffs_browser: 
	python yaffs_browser.py


the yaffs browser is a gui interface used for browsing a yaffs emfile.
python and tkinter must be installed for browser to run.
	sudo apt-get install python python-tk
 
the yaffs_browser.py file must be run in yaffs/direct/python/ directory due to linking issues with python. 

browsing other emfiles
	yaffs will mount the emfile in the same directory as the browser as default 
	to look at a emfile in a diffrent directory create a symbolic link to the emfile you wish to browse.
	ln -s [target path] [emfile name]



TODO list
	Add scroll bars to all windows.
	Add Ctrl + S short cut to save a file
	Make the brower and editor window entry and list boxes resize when the window is maximized. 

