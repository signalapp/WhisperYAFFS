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
# $Id: Makefile,v 1.1 2004-11-03 08:29:28 charles Exp $

CFLAGS =    -Wall -DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_SHORT_NAMES_IN_RAM -DYAFFS2_DEFINES -g
#CFLAGS+=   -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -Wmissing-declarations
#CFLAGS+=   -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Winline


DIRECTTESTOBJS = dtest.o yaffs_ecc.o yaffscfg.o yaffsfs.o yaffs_ramdisk.o yaffs_guts.o yaffs_fileem.o yaffs_packedtags.o yaffs_tagscompat.o

BOOTTESTOBJS = bootldtst.o yboot.o yaffs_fileem.o nand_ecc.o

ALLOBJS =  dtest.o nand_ecc.o yaffscfg.o yaffs_fileem.o yaffsfs.o yaffs_ramdisk.o bootldtst.o yboot.o 

SYMLINKS = devextras.h yaffs_ecc.c yaffs_ecc.h yaffs_guts.c yaffs_guts.h yaffsinterface.h yportenv.h yaffs_tagscompat.c yaffs_tagscompat.h

all: directtest boottest

$(ALLOBJS): %.o: %.c
	gcc -c $(CFLAGS) $< -o $@

$(SYMLINKS):
	ln -s ../$@ $@

directtest: $(SYMLINKS) $(DIRECTTESTOBJS)
	gcc -o $@ $(DIRECTTESTOBJS)


boottest: $(SYMLINKS) $(BOOTTESTOBJS)
	gcc -o $@ $(BOOTTESTOBJS)


clean:
	rm -f $(ALLOBJS) core
