# Main Makefile for out-of-tree yaffs2 .ko building
#
# You can make two flavours of the .ko
#  make YAFFS_CURRENT=1  : makes yaffs2.ko using the current version code
#  make                  : makes yaffs2multi.ko using the multi-version code
#
# YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
#
# Copyright (C) 2002-2010 Aleph One Ltd.
#   for Toby Churchill Ltd and Brightstar Engineering
#
# Created by Charles Manning <charles@aleph1.co.uk>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.

ifdef YAFFS_CURRENT
	YAFFS_O := yaffs2.o
else
	YAFFS_O := yaffs2multi.o
endif

ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS += -DYAFFS_OUT_OF_TREE

	obj-m := $(YAFFS_O)

	yaffs2-objs := yaffs_mtdif.o yaffs_mtdif2_single.o
	yaffs2-objs += yaffs_mtdif1_single.o yaffs_packedtags1.o
	yaffs2-objs += yaffs_ecc.o yaffs_vfs_single.o yaffs_guts.o
	yaffs2-objs += yaffs_crypto.o yaffs_pbkdf2.o
	yaffs2-objs += yaffs_packedtags2.o
	yaffs2-objs += yaffs_tagscompat.o yaffs_tagsvalidity.o
	yaffs2-objs += yaffs_checkptrw.o yaffs_nand.o
	yaffs2-objs += yaffs_checkptrw.o yaffs_nand.o yaffs_nameval.o
	yaffs2-objs += yaffs_allocator.o yaffs_bitmap.o yaffs_attribs.o
	yaffs2-objs += yaffs_yaffs1.o
	yaffs2-objs += yaffs_yaffs2.o
	yaffs2-objs += yaffs_verify.o

	yaffs2multi-objs := yaffs_mtdif.o yaffs_mtdif2_multi.o
	yaffs2multi-objs += yaffs_mtdif1_multi.o yaffs_packedtags1.o
	yaffs2multi-objs += yaffs_ecc.o yaffs_vfs_multi.o yaffs_guts.o
	yaffs2multi-objs += yaffs_crypto.o yaffs_pbkdf2.o
	yaffs2multi-objs += yaffs_packedtags2.o
	yaffs2multi-objs += yaffs_tagscompat.o yaffs_tagsvalidity.o
	yaffs2multi-objs += yaffs_checkptrw.o yaffs_nand.o
	yaffs2multi-objs += yaffs_checkptrw.o yaffs_nand.o yaffs_nameval.o
	yaffs2multi-objs += yaffs_allocator.o yaffs_bitmap.o yaffs_attribs.o
	yaffs2multi-objs += yaffs_yaffs1.o
	yaffs2multi-objs += yaffs_yaffs2.o
	yaffs2multi-objs += yaffs_verify.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

modules default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

mi modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
endif
