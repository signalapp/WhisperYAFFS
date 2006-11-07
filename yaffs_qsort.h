/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system.
 * yaffs_qsort.h: Interface to BSD-licensed qsort routine.
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * $Id: yaffs_qsort.h,v 1.2 2006-11-07 23:20:09 charles Exp $
 */

#ifndef __YAFFS_QSORT_H__
#define __YAFFS_QSORT_H__

extern void qsort (void *const base, size_t total_elems, size_t size,
                   int (*cmp)(const void *, const void *));

#endif
