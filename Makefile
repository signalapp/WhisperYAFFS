#Makefile for YAFFS2 as a module
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
#
# $Id: Makefile,v 1.5 2005-08-01 20:52:35 luc Exp $
#

## Change or override  KERNELDIR to your kernel
## comment out USE_xxxx if you don't want these features.

KERNELDIR = /usr/src/kernels/2.6.11-1.1369_FC4-i686

## Change if you are using a cross-compiler
MAKETOOLS = 

CC=$(MAKETOOLS)gcc
LD=$(MAKETOOLS)ld

# Configurations...
# Comment out the stuff you don't want.
#

# CONFIG_YAFFS_YAFFS1
# This adds the yaffs file system support for working with a
# a 512 byte / page NAND mtd.

USE_MTD = -DCONFIG_YAFFS_YAFFS1

# CONFIG_YAFFS_YAFFS2
# This adds the yaffs2 file system support for working with a 
# 2048 or larger byte / page NAND mtd.

USE_MTD_2 = -DCONFIG_YAFFS_YAFFS2


# CONFIG_YAFFS_RAM_ENABLED.
# This adds the yaffsram file system support. Nice for testing on x86, but uses 2MB of RAM.
# Don't enable for NAND-based targets.

# USE_RAM_FOR_TEST = -DCONFIG_YAFFS_RAM_ENABLED

# CONFIG_YAFFS2_RAM_ENABLED.
# This adds the yaffs2ram file system support. Nice for testing on x86, but uses 2MB of RAM.
# Don't enable for NAND-based targets.

# USE_RAM_FOR_TEST_2 = -DCONFIG_YAFFS2_RAM_ENABLED

# CONFIG_YAFFS_USE_NANDECC
# This enables the ECC functions of the generic MTD-NAND driver.
# This will not work if you are using the old mtd.
#
# NB UseNANDECC does not work at present with yaffsram.

USE_NANDECC = -DCONFIG_YAFFS_USE_NANDECC

# CONFIG_YAFFS_ECC_WRONG_ORDER
# This makes yaffs_ecc.c use the same ecc byte order as
# Steven Hill's nand_ecc.c. If not set, then you get the
# same ecc byte order as SmartMedia.

#USE_WRONGECC = -DCONFIG_YAFFS_ECC_WRONG_ORDER

# CONFIG_YAFFS_USE_HEADER_FILE_SIZE
# When the flash is scanned, two file sizes are constructed:
# * The size taken from the object header for the file.
# * The size figured out by scanning the data chunks.
# If this option is enabled, then the object header size is used, otherwise the scanned size is used.
# Suggest leaving this disabled.

#USE_HEADER_FILE_SIZE = -DCONFIG_YAFFS_USE_HEADER_FILE_SIZE

#CONFIG_YAFFS_DISABLE_CHUNK_ERASED_CHECK
# Enabling this turns off the test that chunks are erased in flash before writing to them.
# this is safe, since the write verification will fail.
# Suggest enabling the test (ie. keep the following line commented) during development to help debug things.

#IGNORE_CHUNK_ERASED = -DCONFIG_YAFFS_DISABLE_CHUNK_ERASED_CHECK

#CONFIG_YAFFS_DISABLE_WRITE_VERIFY
# I am severely reluctant to provide this config. Disabling the verification is not a good thing to do
# since NAND writes can fail silently.
# Disabling the write verification will cause your teeth to rot, rats to eat your corn and give you split ends.
# You have been warned. ie. Don't uncomment the following line.

#IGNORE_WRITE_VERIFY = -DCONFIG_YAFFS_DISBLE_WRITE_VERIFY

#CONFIG_YAFFS_SHORT_NAMES_IN_RAM
# If this config is set, then short names are stored with the yaffs_Object
# This costs an extra 16 bytes of RAM per object, but makes look-ups faster.

ENABLE_SHORT_NAMES_IN_RAM = -DCONFIG_SHORT_NAMES_IN_RAM

# End of configuration options.

WIERD_COMPILE_CONFIGS = -DNR_IRQS=1 -DNR_IRQ_VECTORS=1

YAFFS_CONFIGS = $(WIERD_COMPILE_CONFIGS) \
                $(USE_RAM_FOR_TEST) $(USE_MTD) $(USE_RAM_FOR_TEST_2) $(USE_MTD_2)\
                $(USE_HEADER_FILE_SIZE) $(IGNORE_CHUNK_ERASED) $(IGNORE_WRITE_VERIFY) \
                $(ENABLE_SHORT_NAMES_IN_RAM) $(USE_NANDECC) $(USE_WRONGECC)


YAFFS2_CONFIGS = $(YAFFS_CONFIGS)

CFLAGS = -D__KERNEL__ -DMODULE $(YAFFS2_CONFIGS)  -I$(KERNELDIR)/include -O2 -g -Wall


OBJS = yaffs_fs.o yaffs_guts.o yaffs_ramem.o yaffs_mtdif.o yaffs_ecc.o yaffs_tagscompat.o yaffs_ramem2k.o yaffs_packedtags2.o yaffs_mtdif2.o yaffs_tagsvalidity.o


all: yaffs.o

$(OBJS): %.o: %.c Makefile
	$(CC) -c $(CFLAGS) $< -o $@

yaffs.o: $(OBJS)
	$(LD) -r $(OBJS) -o $@

clean:
	rm -f $(OBJS) core
