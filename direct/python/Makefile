# Makefile for YAFFS direct stress tests
#
#
# YAFFS: Yet another Flash File System. A NAND-flash specific file system.
#
# Copyright (C) 2003 Aleph One Ltd.
#
#
# Created by Charles Manning <charles@aleph1.co.uk>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# NB Warning this Makefile does not include header dependencies.
#
# $Id: Makefile,v 1.1 2009-10-08 01:58:52 charles Exp $

#EXTRA_COMPILE_FLAGS = -DYAFFS_IGNORE_TAGS_ECC

CFLAGS =      -DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_SHORT_NAMES_IN_RAM -DCONFIG_YAFFS_YAFFS2  
CFLAGS +=     -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES -DNO_Y_INLINE
CFLAGS +=    -Wall -g $(EXTRA_COMPILE_FLAGS) -Wstrict-aliasing 
#CFLAGS +=    -fno-strict-aliasing
CFLAGS +=    -O0
#CFLAGS +=    -DVALGRIND_TEST

#CFLAGS+=   -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -Wmissing-declarations
#CFLAGS+=   -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Winline


COMMONTESTOBJS = yaffscfg2k.o yaffs_ecc.o yaffs_fileem.o yaffs_fileem2k.o yaffsfs.o yaffs_guts.o \
		 yaffs_packedtags1.o yaffs_ramdisk.o yaffs_ramem2k.o \
		 yaffs_tagscompat.o yaffs_packedtags2.o yaffs_tagsvalidity.o yaffs_nand.o \
		 yaffs_checkptrw.o  yaffs_qsort.o\
		 yaffs_norif1.o  ynorsim.o


YAFFSTESTOBJS  = $(COMMONTESTOBJS) yaffs_wrap.o yaffs_swig_helper.o


ALLOBJS = $(sort $(YAFFSTESTOBJS))

YAFFSSYMLINKS = devextras.h yaffs_ecc.c yaffs_ecc.h yaffs_guts.c yaffs_guts.h yaffsinterface.h yportenv.h yaffs_tagscompat.c yaffs_tagscompat.h \
          yaffs_packedtags1.c yaffs_packedtags1.h yaffs_packedtags2.c yaffs_packedtags2.h  yaffs_nandemul2k.h \
          yaffs_nand.c yaffs_nand.h yaffs_getblockinfo.h \
          yaffs_tagsvalidity.c yaffs_tagsvalidity.h yaffs_checkptrw.h yaffs_checkptrw.c \
          yaffs_qsort.c yaffs_qsort.h

YAFFSDIRECTSYMLINKS =  yaffscfg2k.c yaffs_fileem2k.c yaffsfs.c yaffs_flashif.h yaffs_flashif2.h\
		       yaffs_fileem2k.h yaffsfs.h yaffs_malloc.h yaffs_ramdisk.h ydirectenv.h \
		       yaffscfg.h yaffs_fileem.c yaffs_flashif.c yaffs_ramdisk.c yaffs_ramem2k.c\
		       yaffs_norif1.c yaffs_norif1.h ynorsim.c ynorsim.h \
		       yaffs_nandif.c yaffs_nandif.h


SYMLINKS = $(YAFFSSYMLINKS) $(YAFFSDIRECTSYMLINKS)

all: _yaffs.so

yaffs_wrap.c: yaffs.i yaffsfs_for_swig.h
	swig -python yaffs.i

yaffsfs_for_swig.h: yaffsfs.h
	cat yaffsfs.h | sed -e "s/YCHAR/char/g" | sed -e "s/YUCHAR/unsigned char/g" | sed -e "s/struct yaffs_stat/struct yaffs_stat_struct/g" > $@

$(ALLOBJS): %.o: %.c
	gcc -c $(CFLAGS) -I/usr/include/python2.6/  -o $@ $<


$(YAFFSSYMLINKS):
	ln -s ../../$@ $@

$(YAFFSDIRECTSYMLINKS):
	ln -s ../$@ $@



_yaffs.so: $(SYMLINKS) yaffs_wrap.c $(ALLOBJS)
	gcc -shared $(ALLOBJS) -o $@




clean:
	rm -f _yaffs.so yaffs.py yaffs.pyc yaffs_wrap.c $(ALLOBJS) core $(YAFFSSYMLINKS) $(YAFFSDIRECTSYMLINKS)
	rm -f *.c.*
	rm -f yaffsfs_for_swig.h
