#Makefile for mkyaffs
#
# NB this is not yet suitable for putting into the kernel tree.
# YAFFS: Yet another FFS. A NAND-flash specific file system. 
#
# Copyright (C) 2002 Aleph One Ltd.
#   for Toby Churchill Ltd and Brightstar Engineering
#
# Created by Charles Manning <charles@aleph1.co.uk>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.

## Change or override  KERNELDIR to your kernel

#KERNELDIR = /usr/src/kernel-headers-2.4.18

CFLAGS =   -I/usr/include -I.. -O2 -Wall -DCONFIG_YAFFS_UTIL
CFLAGS+=   -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -Wmissing-declarations
CFLAGS+=   -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Winline

## Change if you are using a cross-compiler
MAKETOOLS = 

CC=$(MAKETOOLS)gcc

MKYAFFS2SOURCES = mkyaffs2image.c
MKYAFFS2LINKS = yaffs_ecc.c yaffs_packedtags2.c yaffs_tagsvalidity.c

MKYAFFS2IMAGEOBJS = $(MKYAFFS2SOURCES:.c=.o) $(MKYAFFS2LINKS:.c=.o)

all: mkyaffs2image

$(MKYAFFS2IMAGEOBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

mkyaffs2image: $(MKYAFFS2IMAGEOBJS)
	$(CC) -o $@ $(MKYAFFS2IMAGEOBJS)

$(MKYAFFS2LINKS):
	ln -s ../$@ $@


clean:
	rm -f $(MKYAFFS2IMAGEOBJS) core
