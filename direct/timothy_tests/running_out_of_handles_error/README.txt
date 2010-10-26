This directory "freezes" the yaffs_tester programs in the state in which they produce a specific bug.

This yaffs_tester testes the bug "error code 0, when yaffs has run out of handles"
the yaffs_tester program may crash with another error before running out of handles.
To fix this use remove the emkfile and use the seed 1288065181

./yaffs_tester -seed 1288065181


for more information on this bug see the error.txt file
