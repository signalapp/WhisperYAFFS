This directory "freezes" the yaffs_tester programs in the state in which they produce a specific bug.

ENOENT error is generated when opening a new file with O_CREATE.
This should not be happening and has been logged as an error.
To regenerate this error remove the emfile and use the seed 1288064149

./yaffs_tester -seed 1288064149
For more information on this bug see the error_log.txt file
