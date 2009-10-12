Using python with yaffsfs
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Herewith a brief session showing yaffs direct being accessed from python.

I tried SWIG first, but ctypes turned out to be way, way easier...

Thanks to the people on NZPUG mailing list that helped with the exercise.

Build libyaffsfs.so with the accompanying Makefile.

$ make libyaffsfs.so

$ python
Python 2.6.2 (release26-maint, Apr 19 2009, 01:56:41) 
[GCC 4.3.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> from yaffsfs import *
>>> yaffs_StartUp()
0
>>> yaffs_mount("/yaffs2")
yaffs: Mounting /yaffs2
yaffs: yaffs_GutsInitialise()
yaffs_ScanBackwards starts  intstartblk 1 intendblk 256...
0 blocks to be sorted...
...done
yaffs_ScanBackwards ends

Block summary
0 blocks have illegal states
Unknown 0 blocks
Needs scanning 0 blocks
Scanning 0 blocks
Empty 256 blocks
Allocating 0 blocks
Full 0 blocks
Dirty 0 blocks
Checkpoint 0 blocks
Collecting 0 blocks
Dead 0 blocks

yaffs: yaffs_GutsInitialise() done.

0
>>> yaffs_open("/yaffs2/xx",66,0666)
yaffs: Tnodes added
Allocated block 1, seq  4097, 255 left
0
>>> yaffs_write(0,"abcdefg",5)
5
>>> b = create_string_buffer("",100)
>>> yaffs_lseek(0,0,0)
0
>>> yaffs_read(0,b,100)
5
>>> print b.value
abcde
