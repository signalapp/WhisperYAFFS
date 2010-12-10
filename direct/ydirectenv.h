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

/*
 * ydirectenv.h: Environment wrappers for YAFFS direct.
 */

#ifndef __YDIRECTENV_H__
#define __YDIRECTENV_H__

// Direct interface

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "yaffs_osglue.h"
#include "yaffs_hweight.h"

#include "assert.h"
#define YBUG() assert(0)
//#define YBUG() do { *((int *)0) =1;} while(0)


#define YCHAR char
#define YUCHAR unsigned char
#define _Y(x) x

#define hweight8(x)	yaffs_hweight8(x)
#define hweight32(x)	yaffs_hweight32(x)

void yaffs_qsort(void *aa, size_t n, size_t es,
        int (*cmp)(const void *, const void *));

#define sort(base, n, sz, cmp_fn, swp) yaffs_qsort(base, n, sz, cmp_fn)
        
#define YAFFS_PATH_DIVIDERS  "/"

#ifdef NO_Y_INLINE
#define Y_INLINE
#else
#define Y_INLINE __inline__
#endif

#define kmalloc(x,flags) yaffs_malloc(x)
#define kfree(x)   yaffs_free(x)
#define vmalloc(x) yaffs_malloc(x)
#define vfree(x) yaffs_free(x)

#define cond_resched()  do {} while(0)

#define yaffs_trace(msk, fmt, ...) do { \
	if(yaffs_trace_mask & ((msk) | YAFFS_TRACE_ALWAYS)) \
		printf("yaffs: " fmt "\n", ##__VA_ARGS__); \
} while(0)


#define YAFFS_LOSTNFOUND_NAME		"lost+found"
#define YAFFS_LOSTNFOUND_PREFIX		"obj"

#include "yaffscfg.h"

#define Y_CURRENT_TIME yaffsfs_CurrentTime()
#define Y_TIME_CONVERT(x) x

#define YAFFS_ROOT_MODE				0666
#define YAFFS_LOSTNFOUND_MODE		0666

#include "yaffs_list.h"

#include "yaffsfs.h"

#endif


