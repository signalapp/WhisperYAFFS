This directory "freezes" the yaffs_tester programs in the state in which they produce a specific bug.

This yaffs_tester testes the bug "error code 0, when yaffs has run out of handles"
the yaffs_tester program may crash with another error before running out of handles.
To fix this make clean the to reseed the rand function producing random names. 

