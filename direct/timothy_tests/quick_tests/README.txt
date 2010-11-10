
Made by Timothy Manning <timothy@yaffs.net> on 04/11/2010


Tests made
	test_yaffs_mount
	test_yaffs_mount_ENODEV
	test_yaffs_mount_ENAMETOOLONG
	test_yaffs_mount_ENOENT
	test_yaffs_mount_EBUSY		//called when trying to mount a new mount point with a mount point already mounted.

	test_yaffs_access
	test_yaffs_access_ENIVAL
	test_yaffs_access_ENOTDIR
	test_yaffs_access_ENOENT

	test_yaffs_close_EBADF

	test_yaffs_ftruncate
	test_yaffs_ftruncate_EBADF
	test_yaffs_ftruncate_ENIVAL
	test_yaffs_ftruncate_EFBIG

	test_yaffs_lseek
	test_yaffs_lseek_EBADF
	test_yaffs_lseek_EINVAL
	test_yaffs_lseek_EFBIG

	test_yaffs_open
	test_yaffs_open_EEXIST
	test_yaffs_open_EISDIR
	test_yaffs_open_ENAMETOOLONG
	test_yaffs_open_ENOENT
	test_yaffs_open_ENOTDIR
	test_yaffs_open_EINVAL
	test_yaffs_open_EINVAL2 //the function open has two modes which can be set, so two tests modes are needed.

	test_yaffs_read
	test_yaffs_read_EBADF
	test_yaffs_read_EINVAL


	test_yaffs_stat
	test_yaffs_stat_ENOENT
	test_yaffs_stat_ENOTDIR

	test_yaffs_fstat
	test_yaffs_fstat_EBADF

	test_yaffs_truncate
	test_yaffs_truncate_ENOTDIR
	test_yaffs_truncate_EISDIR
	test_yaffs_truncate_ENOENT
	test_yaffs_truncate_ENIVAL
	test_yaffs_truncate_EFBIG

	test_yaffs_unlink
	test_yaffs_unlink_EISDIR
	test_yaffs_unlink_ENAMETOOLONG
	test_yaffs_unlink_ENOENT
	test_yaffs_unlink_ENOTDIR
	test_yaffs_unlink_ENOENT

	test_yaffs_unmount
	test_yaffs_unmount_ENOENT
	test_yaffs_unmount_ENAMETOOLONG
	test_yaffs_umount_EBUSY	//cause by having a file handle open and then trying to unmount yaffs.

	test_yaffs_write
	test_yaffs_write_EBADF

	

Tests to add
	test_yaffs_mount_EACCES		//Cannot be generated with yaffs.
	test_yaffs_mount_EINVAL		//Cannot be generated with yaffs.
	test_yaffs_mount_ELOOP		//Cannot be generated with yaffs.
	test_yaffs_mount_EMFILE		//Cannot be generated with yaffs.
	test_yaffs_mount_ENOTDIR	//Cannot be generated with yaffs.

	test_yaffs_umount_ENODEV	//Cannot be generated with yaffs.
	test_yaffs_umount_ENOENT	//Cannot be generated with yaffs.
			

	test_yaffs_open_EACCES
	test_yaffs_open_ENOSPC
	test_yaffs_open_ELOOP   //Too many symbolic links were encountered in resolving pathname
	test yaffs_open_running_out_of_handles error

	test_yaffs_close	//This function has already been called by the time this test is reached.
	
	test_yaffs_stat_EBADF
	test_yaffs_stat_ELOOP
	test_yaffs_stat_EACCES
	test_yaffs_stat_ENAMETOOLONG
	test_yaffs_stat_ENOTDIR
	 
	test_yaffs_read_EISDIR 		//Cannot be generated with yaffs.
	test_yaffs_read_EFBIG
	test what happens if you read off the end of the file?

	test_yaffs_write_EFBIG
	test_yaffs_write_EINVAL
	What happens when you run out of space?

	test_yaffs_unlink_EACCES
	test_yaffs_unlink_ELOOP
	test_yaffs_unlink_ENOMEM

	test_yaffs_stat_EACCES
	test_yaffs_stat_ELOOP

	test_yaffs_access_EACCESS
	test_yaffs_access_ELOOP
	test_yaffs_access_ENAMETOOLONG
	test_yaffs_access_ENOENT_generated_with_a_dangling_symbloic_link

	test_yaffs_ftruncate_EACCES	
	test_yaffs_ftruncate_EISDIR	//Cannot be generated with yaffs.
	test_yaffs_ftruncate_ELOOP	//Cannot be generated with yaffs.
	test_yaffs_ftruncate_ENOENT	//Cannot be generated with yaffs.
	test_yaffs_ftruncate_ENOTDIR	//Cannot be generated with yaffs.

	test_yaffs_truncate_EACCES
	test_yaffs_truncate_ELOOP
	test_yaffs_truncate_ENAMETOOLONG
	Add a truncate function for truncating a file size to -1.

	What happens if a handle is opened to a file and the file is then deleted?
	Check to see if yaffs generates an error code for no reason.
	What happens when a file is opened with no modes set?
	Add a test where a directory is moved. /fluffy/box. move "fluffy" to "/fluffy/frog". 
	What happens when yaffs is unmounted twice?

	What happens when open a handle, unmount yaffs and then try to use the handle? 

How to add a test
	First create the test .c and .h file.
	The file name and test function name should be the same. 
	This name should follow one of these formats: 
	Test_yaffs_[function of yaffs which is been tested]
	Test_yaffs_[function of yaffs which is been tested]_[error trying to be generated]
	
	The .c file needs to contain two functions.
	The first function needs to contain the code for the main test and will 
	return -1 on a failure and 0 or greater on a success.
	The second function needs contain the code for cleaning up after the test. 
	Cleaning up may include closing some open handles, recreating a file, ect. 
	This second function needs to return -1 on a failure and 0 or greater on success.

	The name of first function needs to be called the same as the file 
	name (without the .c or .h)
	The second function's name needs be the same as the first function but 
	with "_clean" added on the end.
	
	So if a test is been created for the yaffs function yaffs_foo() then 
	create these files
	test_yaffs_foo.c
		Contains int test_yaffs_foo(void); int test_yaffs_foo_clean(void);
	test_yaffs_foo.h
		Which includes "lib.h", "yaffsfs.h" header files.

	Next write the test code in these files then add these files to the Makefile.

	Add the name of the test files' object file (test_yaffs_foo.o ) to the 
	TESTFILES tag around line 50 of the Makefile.	

	Now add the test functions to the test_list[] array in quick_tests.h
	The order of the tests matters. The idea is to test each yaffs_function 
	individualy and only using tested yaffs_components before using this new 
	yaffs_function. 
	This array consists of: 
	{[test function], [the clean function], [name of the tests which will be printed when the test fails]},	
	
	So add this line to the test_list[]: {test_yaffs_foo, test_yaffs_foo_clean, "test_yaffs_foo"},

	Also include the test's .h file in the quick_test.h file: #include "test_yaffs_foo.h"
	
	The test file should now make and run(you may need to make clean first). 



	PS: yaffs_foo() is a made up function for this README (in case you want 
	to find this function in yaffs). 






