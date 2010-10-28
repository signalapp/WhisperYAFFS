
This program tries to open a new file in yaffs. The yaffs_open() function returns a -1 handle and gives an error of EACCES.
This bug only seems to happen when the emfile has a lot of files in it.
