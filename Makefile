# Main Makefile for out-of-tree yaffs2.ko building
#
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


ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS += -DYAFFS_OUT_OF_TREE

	obj-m := yaffs2.o yaffs2multi.o

	yaffs2-objs := yaffs_mtdif.o yaffs_mtdif2.o
	yaffs2-objs += yaffs_mtdif1.o yaffs_packedtags1.o
	yaffs2-objs += yaffs_ecc.o yaffs_vfs.o yaffs_guts.o
	yaffs2-objs += yaffs_packedtags2.o
	yaffs2-objs += yaffs_tagscompat.o yaffs_tagsvalidity.o
	yaffs2-objs += yaffs_checkptrw.o yaffs_nand.o
	yaffs2-objs += yaffs_checkptrw.o yaffs_nand.o yaffs_nameval.o
	yaffs2-objs += yaffs_allocator.o yaffs_bitmap.o
	yaffs2-objs += yaffs_yaffs1.o
	yaffs2-objs += yaffs_yaffs2.o
	yaffs2-objs += yaffs_verify.o

	yaffs2multi-objs := yaffs_mtdif.o yaffs_mtdif2.o
	yaffs2multi-objs += yaffs_mtdif1.o yaffs_packedtags1.o
	yaffs2multi-objs += yaffs_ecc.o yaffs_vfs_multi.o yaffs_guts.o
	yaffs2multi-objs += yaffs_packedtags2.o
	yaffs2multi-objs += yaffs_tagscompat.o yaffs_tagsvalidity.o
	yaffs2multi-objs += yaffs_checkptrw.o yaffs_nand.o
	yaffs2multi-objs += yaffs_checkptrw.o yaffs_nand.o yaffs_nameval.o
	yaffs2multi-objs += yaffs_allocator.o yaffs_bitmap.o
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
