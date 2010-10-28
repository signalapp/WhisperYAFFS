Yaffs bug

Yaffs' mkdir function will not work if the path ends in a slash.
i.e. /yaffs2/new_dir/
This bug is known and is not likely to be fixed. 
To get around this bug remove the slash on the end.
i.e. /yaffs2/new_dir

Running the program with any seed will work.
./yaffs_tester


