
tries to open a new file in yaffs. yaffs_open returns a -1 handle. and gives an error of EACCES.
this bug only seems to happen when the emfile has a lot of files in it.
