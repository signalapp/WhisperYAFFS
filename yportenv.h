/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 * yportenv.h: Portable services used by yaffs. This is done to allow
 * simple migration from kernel space into app space for testing.
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
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 *
 * $Id: yportenv.h,v 1.1 2004-11-03 08:14:07 charles Exp $
 *
 */
 
#ifndef __YPORTENV_H__
#define __YPORTENV_H__


#if defined CONFIG_YAFFS_WINCE

// CONFIG_YAFFS_WINCE
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define CONFIG_YAFFS_CASE_INSENSITIVE
#define CONFIG_YAFFS_UNICODE

#define YMALLOC(x) malloc(x)
#define YFREE(x)   free(x)


#define YINFO(s) YPRINTF(( __FILE__ " %d %s\n",__LINE__,s))
#define YALERT(s) YINFO(s)

#include <windows.h>

#define YPRINTF(x)	RETAILMSG(1,x)

// Always pass the sum compare to overcome the case insensitivity issue
#define yaffs_SumCompare(x,y) ((x) == (y))

#ifdef CONFIG_YAFFS_UNICODE
#define YCHAR					WCHAR
#define YUCHAR					WCHAR
#define	_Y(a)					L##a
#define yaffs_toupper(a)		towupper(a)
#define yaffs_strcmp(a,b)		_wcsicmp(a,b)
#define yaffs_strcpy(a,b)		wcscpy(a,b)
#define yaffs_strncpy(a,b,c)	wcsncpy(a,b,c)
#define yaffs_strlen(s)			wcslen(s)
#define yaffs_sprintf			swprintf
#else
#define YCHAR					CHAR
#define YUCHAR					UCHAR
#define _Y(a)					a
#define	yaffs_toupper(a)		toupper(a)
#define yaffs_strcmp(a,b)		_stricmp(a,b)
#define yaffs_strcpy(a,b)		strcpy(a,b)
#define yaffs_strncpy(a,b,c)	strncpy(a,b,c)
#define yaffs_strlen(s)			strlen(s)
#define yaffs_sprintf			sprintf
#endif

#define YAFFS_LOSTNFOUND_NAME		_Y("Lost Clusters")
#define YAFFS_LOSTNFOUND_PREFIX		_Y("OBJ")


#define u_char unsigned char
#define loff_t int
#define S_IFDIR						04000

#define S_ISFIFO(x) 0
#define S_ISCHR(x) 0
#define S_ISBLK(x) 0
#define S_ISSOCK(x) 0

extern unsigned yfsd_U32FileTimeNow(void);

#define CURRENT_TIME				 yfsd_U32FileTimeNow()
#define YAFFS_ROOT_MODE				FILE_ATTRIBUTE_ARCHIVE
#define YAFFS_LOSTNFOUND_MODE		FILE_ATTRIBUTE_HIDDEN


#define TENDSTR L"\r\n"
#define TSTR(x) TEXT(x)
#define TOUT(x) RETAILMSG(1, x)


#elif  defined __KERNEL__



// Linux kernel
#include "linux/kernel.h"
#include "linux/mm.h"
#include "linux/string.h"
#include "linux/slab.h"


#define YAFFS_LOSTNFOUND_NAME		"lost+found"
#define YAFFS_LOSTNFOUND_PREFIX		"obj"

//#define YPRINTF(x) printk x
#define YMALLOC(x) kmalloc(x,GFP_KERNEL)
#define YFREE(x)   kfree(x)

#define YAFFS_ROOT_MODE				0666
#define YAFFS_LOSTNFOUND_MODE		0666


#define yaffs_SumCompare(x,y) ((x) == (y))
#define YCHAR					char
#define YUCHAR					unsigned char
#define _Y(a)					a
#define	yaffs_toupper(a)		toupper(a)
#define yaffs_strcmp(a,b)		strcmp(a,b)
#define yaffs_strcpy(a,b)		strcpy(a,b)
#define yaffs_strncpy(a,b,c)	strncpy(a,b,c)
#define yaffs_strlen(s)			strlen(s)
#define yaffs_sprintf			sprintf

#define TENDSTR "\n"
#define TSTR(x) KERN_DEBUG x
#define TOUT(p) printk p


#elif defined CONFIG_YAFFS_DIRECT

// Direct interface

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define YMALLOC(x) malloc(x)
#define YFREE(x)   free(x)


//#define YINFO(s) YPRINTF(( __FILE__ " %d %s\n",__LINE__,s))
//#define YALERT(s) YINFO(s)


#define TENDSTR "\n"
#define TSTR(x) x
#define TOUT(p) printf p


#define YAFFS_LOSTNFOUND_NAME		"lost+found"
#define YAFFS_LOSTNFOUND_PREFIX		"obj"
//#define YPRINTF(x) printf x

#include "yaffscfg.h"

#define CURRENT_TIME yaffsfs_CurrentTime()
#define YAFFS_ROOT_MODE				0666
#define YAFFS_LOSTNFOUND_MODE		0666

#define yaffs_SumCompare(x,y) ((x) == (y))
#define YCHAR					char
#define YUCHAR					unsigned char
#define _Y(a)					a
#define	yaffs_toupper(a)		toupper(a)
#define yaffs_strcmp(a,b)		strcmp(a,b)
#define yaffs_strcpy(a,b)		strcpy(a,b)
#define yaffs_strncpy(a,b,c)	strncpy(a,b,c)
#define yaffs_strlen(s)			strlen(s)
#define yaffs_sprintf			sprintf

#elif defined CONFIG_YAFFS_UTIL

// Stuff for YAFFS utilities

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define YMALLOC(x) malloc(x)
#define YFREE(x)   free(x)


//#define YINFO(s) YPRINTF(( __FILE__ " %d %s\n",__LINE__,s))
//#define YALERT(s) YINFO(s)


#define TENDSTR "\n"
#define TSTR(x) x
#define TOUT(p) printf p


#define YAFFS_LOSTNFOUND_NAME		"lost+found"
#define YAFFS_LOSTNFOUND_PREFIX		"obj"
//#define YPRINTF(x) printf x


#define CURRENT_TIME 0
#define YAFFS_ROOT_MODE				0666
#define YAFFS_LOSTNFOUND_MODE		0666

#define yaffs_SumCompare(x,y) ((x) == (y))
#define YCHAR					char
#define YUCHAR					unsigned char
#define _Y(a)					a
#define	yaffs_toupper(a)		toupper(a)
#define yaffs_strcmp(a,b)		strcmp(a,b)
#define yaffs_strcpy(a,b)		strcpy(a,b)
#define yaffs_strncpy(a,b,c)	strncpy(a,b,c)
#define yaffs_strlen(s)			strlen(s)
#define yaffs_sprintf			sprintf

#else
// Should have specified a configuration type
#error Unknown configuration

#endif 


extern unsigned yaffs_traceMask;

#define YAFFS_TRACE_ERROR		0x0001
#define YAFFS_TRACE_OS			0x0002
#define YAFFS_TRACE_ALLOCATE	0x0004
#define YAFFS_TRACE_SCAN		0x0008
#define YAFFS_TRACE_BAD_BLOCKS	0x0010
#define YAFFS_TRACE_ERASE		0x0020
#define YAFFS_TRACE_GC			0x0040
#define YAFFS_TRACE_DELETION	0x0080
#define YAFFS_TRACE_TRACING		0x0100
#define YAFFS_TRACE_ALWAYS		0x0200
#define YAFFS_TRACE_WRITE		0x0400
#define YAFFS_TRACE_BUG			0x8000

#define T(mask,p) do{ if((mask) & (yaffs_traceMask | YAFFS_TRACE_ERROR)) TOUT(p);} while(0) 

//#undef YINFO


//#define YINFO(s) YPRINTF((KERN_DEBUG __FILE__ " %d %s\n",__LINE__,s))
//#define YALERT(s) YINFO(s)
#ifdef CONFIG_YAFFS_WINCE
#define YBUG() T(YAFFS_TRACE_BUG,(TSTR("==>> yaffs bug: %s %d" TENDSTR),TEXT(__FILE__),__LINE__))
#else
#define YBUG() T(YAFFS_TRACE_BUG,(TSTR("==>> yaffs bug: " __FILE__ " %d" TENDSTR),__LINE__))
#endif
#endif
