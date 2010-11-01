/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __Y_NORSIM_H__
#define __Y_NORSIM_H__

#include "yaffs_guts.h"

void ynorsim_rd32(u32 *addr, u32 *data, int nwords);
void ynorsim_wr32(u32 *addr, u32 *data, int nwords);
void ynorsim_erase(u32 *addr);
void ynorsim_shutdown(void);
void ynorsim_initialise(void);
u32 * ynorsim_get_base(void);

#endif
