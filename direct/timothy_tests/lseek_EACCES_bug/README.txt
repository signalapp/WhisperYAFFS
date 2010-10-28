

yaffs_tester.c lseeks and writes to a file in for loop then suddenly lseek( on line 238) returns the error EACCES.
The handle to the file has not changed nor has the handle been closed.
There is no reason for this error to happen and is therefor logged as a bug.

if the emfile contains quite a few files then the same lseek will return a error code of 0 (see lseek_error_code_0_bug directory).   
