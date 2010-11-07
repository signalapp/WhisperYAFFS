
Made by Timothy Manning <timothy@yaffs.net> on 04/11/2010


the yaffs_open function has been covered by tests
the yaffs_close function has been covered by tests

tests made
	test_yaffs_mount
	test_yaffs_access
	test_yaffs_close
	test_yaffs_close_EBADF
	test_yaffs_ftruncate
	test_yaffs_lseek
	test_yaffs_open
	test_yaffs_open_EEXIST
	test_yaffs_open_EISDIR
	test_yaffs_open_ENAMETOOLONG
	test_yaffs_open_ENOENT
	test_yaffs_open_ENOTDIR
	test_yaffs_read
	test_yaffs_stat
	test_yaffs_truncate
	test_yaffs_unlink_EISDIR
	test_yaffs_unlink_ENAMETOOLONG
	test_yaffs_unlink_ENOENT
	test_yaffs_unlink_ENOTDIR
	test_yaffs_write
tests to add
	test_yaffs_mount_EACCES
	test_yaffs_mount_EINVAL
	test_yaffs_mount_ELOOP
	test_yaffs_mount_EMFILE
	test_yaffs_mount_ENAMETOOLONG
	test_yaffs_mount_ENOENT
	test_yaffs_mount_ENOTDIR
	test_yaffs_open_EACCES
	test_yaffs_open_ENOSPC
	test_yaffs_open_ELOOP   Too many symbolic links were encountered in resolving pathname
	test_yaffs_close
	test yaffs_open_running_out_of_handles error
	test_yaffs_unlink_EACCES
	test_yaffs_unlink_ELOOP
	test_yaffs_unlink_ENOENT
	test_yaffs_unlink_ENOMEM
	check to see if an error code is generated when there isn't an error
	test_yaffs_access_EACCESS
	test_yaffs_access_ELOOP
	test_yaffs_access_ENAMETOOLONG
	test_yaffs_access_ENOENT
	test_yaffs_access_ENOTDIR
	test_yaffs_access_ENIVAL	//mode is incorrect.


How to add a test
	First create the test .c and .h file.
	The file name and test function name should be the same. 
	This name should follow one of these formats: 
	Test_yaffs_[function of yaffs which is been tested]
	Test_yaffs_[function of yaffs which is been tested]_[error trying to be generated]
	
	The .c file needs to contain two functions.
	The first function needs to contain the code for the main test and will return -1 on a failure and 0 or greater on a success.
	The second function needs contain the code for cleaning up after the test. Cleaning up may include closing some open handles, recreating a file, ect. 
	This second function needs to return -1 on a failure and 0 or greater on success.

	The name of first function needs to be called the same as the file name (without the .c or .h)
	The second function's name needs be the same as the first function but with "_clean" added on the end.
	
	So if a test is been created for the yaffs function yaffs_fish() then create these files
	Test_yaffs_fish.c
		Contains int test_yaffs_fish(void); int test_yaffs_fish_clean(void);
	Test_yaffs_fish.h
		Which includes "lib.h", "yaffsfs.h" header files.

	Next write the test code in these files then add these files to the Makefile.

	Add the name of the test files' object file (test_yaffs_fish.o ) to the TESTFILES tag around line 50 of the Makefile.	



	Now add the test functions to the test_list[] array in quick_tests.h
	The order of the tests matters. The idea is to test each yaffs_function individualy and only using tested yaffs_components before using this new yaffs_function. 
	This array consists of: {[test function], [the clean function], [name of the tests which will be printed when the test fails]},	
	
	So add this line to the test_list[]: {test_yaffs_fish, test_yaffs_fish_clean, "test_yaffs_fish"},

	Also include the test's .h file in the quick_test.h file: #include "test_yaffs_fish.h"

	The test file should now make and run. 
	
	
	



	PS: yaffs_fish() is a made up function for this README (in case you want to try and find this function in yaffs). 


BUGS
	Needing to include int random_seed; and int simulate_power_failure = 0; in any main program using yaffsfs.h
	bug with opening a file with a name of 1,000,000 char long with no errors.
	bug with unlinking a file with 1,000,000 get the error ENOENT but should be geting ENAMETOOLONG. 
	
	FIXED-ENOSPC error in programs test_yaffs_open_ENOTDIR and test_yaffs_open_ENOENT.
	FIXED-ENOENT been returned by yaffs_read but the handle is good and the yaffs_open function does not return an error.




