/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
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
 * This file is just holds extra declarations of macros that would normally
 * be providesd in the Linux kernel. These macros have been written from
 * scratch but are functionally equivalent to the Linux ones.
 *
 */

#ifndef __EXTRAS_H__
#define __EXTRAS_H__

#if defined WIN32
#define __inline__ __inline
#define new newHack
#endif

#if !(defined __KERNEL__) || (defined WIN32)

/* Definition of types */
typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned __u32;

/*
 * This is a simple doubly linked list implementation that matches the 
 * way the Linux kernel doubly linked list implementation works.
 */

struct list_head {
	struct list_head *next; /* next in chain */
	struct list_head *prev; /* previous in chain */
};


/* Initialise a list head to an empty list */
#define INIT_LIST_HEAD(p) \
do { \
 (p)->next = (p);\
 (p)->prev = (p); \
} while(0)


/* Add an element to a list */
static __inline__ void list_add(struct list_head *newEntry, 
				struct list_head *list)
{
	struct list_head *listNext = list->next;
	
	list->next = newEntry;
	newEntry->prev = list;
	newEntry->next = listNext;
	listNext->prev = newEntry;
	
}


/* Take an element out of its current list, with or without
 * reinitialising the links.of the entry*/
static __inline__ void list_del(struct list_head *entry)
{
	struct list_head *listNext = entry->next;
	struct list_head *listPrev = entry->prev;
	
	listNext->prev = listPrev;
	listPrev->next = listNext;
	
}

static __inline__ void list_del_init(struct list_head *entry)
{
	list_del(entry);
	entry->next = entry->prev = entry;
}


/* Test if the list is empty */
static __inline__ int list_empty(struct list_head *entry)
{
	return (entry->next == entry);
}


/* list_entry takes a pointer to a list entry and offsets it to that
 * we can find a pointer to the object it is embedded in.
 */
 
 
#define list_entry(entry, type, member) \
	((type *)((char *)(entry)-(unsigned long)(&((type *)NULL)->member)))


/* list_for_each and list_for_each_safe  iterate over lists.
 * list_for_each_safe uses temporary storage to make the list delete safe
 */

#define list_for_each(itervar, list) \
	for (itervar = (list)->next; itervar != (list); itervar = itervar->next )

#define list_for_each_safe(itervar,saveVar, list) \
	for (itervar = (list)->next, saveVar = (list)->next->next; itervar != (list); \
	 itervar = saveVar, saveVar = saveVar->next)

/* File types */

#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DR_WHT		14


#ifndef WIN32
#include <sys/stat.h>
#endif

/*
 * Attribute flags.
 */
#define ATTR_MODE	1
#define ATTR_UID	2
#define ATTR_GID	4
#define ATTR_SIZE	8
#define ATTR_ATIME	16
#define ATTR_MTIME	32
#define ATTR_CTIME	64


struct iattr {
	unsigned int ia_valid;
	unsigned ia_mode;
	unsigned ia_uid;
	unsigned ia_gid;
	unsigned ia_size;
	unsigned ia_atime;
	unsigned ia_mtime;
	unsigned ia_ctime;
	unsigned int ia_attr_flags;
};

#define KERN_DEBUG

#else

#ifndef WIN32
#include <linux/types.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/stat.h>
#endif

#endif

#if defined WIN32
#undef new
#endif

#endif
