# Makefile for YAFFS direct test
#
# Copyright (C) 2003 Aleph One Ltd.
#
# Created by Charles Manning <charles@aleph1.co.uk>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# NB Warning this Makefile does not include header dependencies.
#
# $Id: Makefile,v 1.4 2004-11-16 02:36:15 charles Exp $

CFLAGS =    -Wall -DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_SHORT_NAMES_IN_RAM -DCONFIG_YAFFS_YAFFS2 -g
#CFLAGS+=   -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -Wmissing-declarations
#CFLAGS+=   -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Winline


DIRECTTESTOBJS = dtest.o yaffscfg2k.o yaffs_ecc.o yaffs_fileem2k.o yaffsfs.o yaffs_guts.o yaffs_packedtags.o yaffs_ramdisk.o yaffs_tagscompat.o

BOOTTESTOBJS = bootldtst.o yboot.o yaffs_fileem.o nand_ecc.o

ALLOBJS =  dtest.o nand_ecc.o yaffscfg.o yaffs_fileem.o yaffsfs.o yaffs_ramdisk.o bootldtst.o yboot.o 

SYMLINKS = devextras.h yaffs_ecc.c yaffs_ecc.h yaffs_guts.c yaffs_guts.h yaffsinterface.h yportenv.h yaffs_tagscompat.c yaffs_tagscompat.h yaffs_packedtags.c yaffs_packedtags.h

#all: directtest2k boottest

all: directtest2k

$(ALLOBJS): %.o: %.c
	gcc -c $(CFLAGS) $< -o $@

$(SYMLINKS):
	ln -s ../$@ $@

directtest2k: $(SYMLINKS) $(DIRECTTESTOBJS)
	gcc -o $@ $(DIRECTTESTOBJS)


boottest: $(SYMLINKS) $(BOOTTESTOBJS)
	gcc -o $@ $(BOOTTESTOBJS)


clean:
	rm -f $(ALLOBJS) core
