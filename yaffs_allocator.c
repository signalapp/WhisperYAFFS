/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */



#include "yaffs_allocator.h"
#include "yaffs_guts.h"
#include "yaffs_trace.h"
#include "yportenv.h"

#ifdef CONFIG_YAFFS_YMALLOC_ALLOCATOR

void yaffs_deinit_raw_tnodes_and_objs(yaffs_dev_t *dev)
{
	dev = dev;
}

void yaffs_init_raw_tnodes_and_objs(yaffs_dev_t *dev)
{
	dev = dev;
}

yaffs_tnode_t *yaffs_alloc_raw_tnode(yaffs_dev_t *dev)
{
	return (yaffs_tnode_t *)YMALLOC(dev->tnode_size);
}

void yaffs_free_raw_tnode(yaffs_dev_t *dev, yaffs_tnode_t *tn)
{
	dev = dev;
	YFREE(tn);
}

void yaffs_init_raw_objs(yaffs_dev_t *dev)
{
	dev = dev;
}

void yaffs_deinit_raw_objs(yaffs_dev_t *dev)
{
	dev = dev;
}

yaffs_obj_t *yaffs_alloc_raw_obj(yaffs_dev_t *dev)
{
	dev = dev;
	return (yaffs_obj_t *) YMALLOC(sizeof(yaffs_obj_t));
}


void yaffs_free_raw_obj(yaffs_dev_t *dev, yaffs_obj_t *obj)
{

	dev = dev;
	YFREE(obj);
}

#else

struct yaffs_tnode_list {
	struct yaffs_tnode_list *next;
	yaffs_tnode_t *tnodes;
};

typedef struct yaffs_tnode_list yaffs_tnodelist_t;

struct yaffs_obj_list_struct {
	yaffs_obj_t *objects;
	struct yaffs_obj_list_struct *next;
};

typedef struct yaffs_obj_list_struct yaffs_obj_list;


struct yaffs_allocator {
	int n_tnodes_created;
	yaffs_tnode_t *free_tnodes;
	int n_free_tnodes;
	yaffs_tnodelist_t *alloc_tnode_list;

	int n_obj_created;
	yaffs_obj_t *free_objs;
	int n_free_objects;

	yaffs_obj_list *allocated_obj_list;
};


static void yaffs_deinit_raw_tnodes(yaffs_dev_t *dev)
{

	struct yaffs_allocator *allocator = (struct yaffs_allocator *)dev->allocator;

	yaffs_tnodelist_t *tmp;

	if(!allocator){
		YBUG();
		return;
	}

	while (allocator->alloc_tnode_list) {
		tmp = allocator->alloc_tnode_list->next;

		YFREE(allocator->alloc_tnode_list->tnodes);
		YFREE(allocator->alloc_tnode_list);
		allocator->alloc_tnode_list = tmp;

	}

	allocator->free_tnodes = NULL;
	allocator->n_free_tnodes = 0;
	allocator->n_tnodes_created = 0;
}

static void yaffs_init_raw_tnodes(yaffs_dev_t *dev)
{
	struct yaffs_allocator *allocator = dev->allocator;

	if(allocator){
		allocator->alloc_tnode_list = NULL;
		allocator->free_tnodes = NULL;
		allocator->n_free_tnodes = 0;
		allocator->n_tnodes_created = 0;
	} else
		YBUG();
}

static int yaffs_create_tnodes(yaffs_dev_t *dev, int n_tnodes)
{
	struct yaffs_allocator *allocator = (struct yaffs_allocator *)dev->allocator;
	int i;
	yaffs_tnode_t *new_tnodes;
	u8 *mem;
	yaffs_tnode_t *curr;
	yaffs_tnode_t *next;
	yaffs_tnodelist_t *tnl;

	if(!allocator){
		YBUG();
		return YAFFS_FAIL;
	}

	if (n_tnodes < 1)
		return YAFFS_OK;


	/* make these things */

	new_tnodes = YMALLOC(n_tnodes * dev->tnode_size);
	mem = (u8 *)new_tnodes;

	if (!new_tnodes) {
		T(YAFFS_TRACE_ERROR,
			(TSTR("yaffs: Could not allocate Tnodes" TENDSTR)));
		return YAFFS_FAIL;
	}

	/* New hookup for wide tnodes */
	for (i = 0; i < n_tnodes - 1; i++) {
		curr = (yaffs_tnode_t *) &mem[i * dev->tnode_size];
		next = (yaffs_tnode_t *) &mem[(i+1) * dev->tnode_size];
		curr->internal[0] = next;
	}

	curr = (yaffs_tnode_t *) &mem[(n_tnodes - 1) * dev->tnode_size];
	curr->internal[0] = allocator->free_tnodes;
	allocator->free_tnodes = (yaffs_tnode_t *)mem;

	allocator->n_free_tnodes += n_tnodes;
	allocator->n_tnodes_created += n_tnodes;

	/* Now add this bunch of tnodes to a list for freeing up.
	 * NB If we can't add this to the management list it isn't fatal
	 * but it just means we can't free this bunch of tnodes later.
	 */

	tnl = YMALLOC(sizeof(yaffs_tnodelist_t));
	if (!tnl) {
		T(YAFFS_TRACE_ERROR,
		  (TSTR
		   ("yaffs: Could not add tnodes to management list" TENDSTR)));
		   return YAFFS_FAIL;
	} else {
		tnl->tnodes = new_tnodes;
		tnl->next = allocator->alloc_tnode_list;
		allocator->alloc_tnode_list = tnl;
	}

	T(YAFFS_TRACE_ALLOCATE, (TSTR("yaffs: Tnodes added" TENDSTR)));

	return YAFFS_OK;
}


yaffs_tnode_t *yaffs_alloc_raw_tnode(yaffs_dev_t *dev)
{
	struct yaffs_allocator *allocator = (struct yaffs_allocator *)dev->allocator;
	yaffs_tnode_t *tn = NULL;

	if(!allocator){
		YBUG();
		return NULL;
	}

	/* If there are none left make more */
	if (!allocator->free_tnodes)
		yaffs_create_tnodes(dev, YAFFS_ALLOCATION_NTNODES);

	if (allocator->free_tnodes) {
		tn = allocator->free_tnodes;
		allocator->free_tnodes = allocator->free_tnodes->internal[0];
		allocator->n_free_tnodes--;
	}

	return tn;
}

/* FreeTnode frees up a tnode and puts it back on the free list */
void yaffs_free_raw_tnode(yaffs_dev_t *dev, yaffs_tnode_t *tn)
{
	struct yaffs_allocator *allocator = dev->allocator;

	if(!allocator){
		YBUG();
		return;
	}

	if (tn) {
		tn->internal[0] = allocator->free_tnodes;
		allocator->free_tnodes = tn;
		allocator->n_free_tnodes++;
	}
	dev->checkpoint_blocks_required = 0; /* force recalculation*/
}



static void yaffs_init_raw_objs(yaffs_dev_t *dev)
{
	struct yaffs_allocator *allocator = dev->allocator;

	if(allocator) {
		allocator->allocated_obj_list = NULL;
		allocator->free_objs = NULL;
		allocator->n_free_objects = 0;
	} else
		YBUG();
}

static void yaffs_deinit_raw_objs(yaffs_dev_t *dev)
{
	struct yaffs_allocator *allocator = dev->allocator;
	yaffs_obj_list *tmp;

	if(!allocator){
		YBUG();
		return;
	}

	while (allocator->allocated_obj_list) {
		tmp = allocator->allocated_obj_list->next;
		YFREE(allocator->allocated_obj_list->objects);
		YFREE(allocator->allocated_obj_list);

		allocator->allocated_obj_list = tmp;
	}

	allocator->free_objs = NULL;
	allocator->n_free_objects = 0;
	allocator->n_obj_created = 0;
}


static int yaffs_create_free_objs(yaffs_dev_t *dev, int n_obj)
{
	struct yaffs_allocator *allocator = dev->allocator;

	int i;
	yaffs_obj_t *new_objs;
	yaffs_obj_list *list;

	if(!allocator){
		YBUG();
		return YAFFS_FAIL;
	}

	if (n_obj < 1)
		return YAFFS_OK;

	/* make these things */
	new_objs = YMALLOC(n_obj * sizeof(yaffs_obj_t));
	list = YMALLOC(sizeof(yaffs_obj_list));

	if (!new_objs || !list) {
		if (new_objs){
			YFREE(new_objs);
			new_objs = NULL;
		}
		if (list){
			YFREE(list);
			list = NULL;
		}
		T(YAFFS_TRACE_ALLOCATE,
		  (TSTR("yaffs: Could not allocate more objects" TENDSTR)));
		return YAFFS_FAIL;
	}

	/* Hook them into the free list */
	for (i = 0; i < n_obj - 1; i++) {
		new_objs[i].siblings.next =
				(struct ylist_head *)(&new_objs[i + 1]);
	}

	new_objs[n_obj - 1].siblings.next = (void *)allocator->free_objs;
	allocator->free_objs = new_objs;
	allocator->n_free_objects += n_obj;
	allocator->n_obj_created += n_obj;

	/* Now add this bunch of Objects to a list for freeing up. */

	list->objects = new_objs;
	list->next = allocator->allocated_obj_list;
	allocator->allocated_obj_list = list;

	return YAFFS_OK;
}

yaffs_obj_t *yaffs_alloc_raw_obj(yaffs_dev_t *dev)
{
	yaffs_obj_t *obj = NULL;
	struct yaffs_allocator *allocator = dev->allocator;

	if(!allocator) {
		YBUG();
		return obj;
	}

	/* If there are none left make more */
	if (!allocator->free_objs)
		yaffs_create_free_objs(dev, YAFFS_ALLOCATION_NOBJECTS);

	if (allocator->free_objs) {
		obj = allocator->free_objs;
		allocator->free_objs =
			(yaffs_obj_t *) (allocator->free_objs->siblings.next);
		allocator->n_free_objects--;
	}

	return obj;
}


void yaffs_free_raw_obj(yaffs_dev_t *dev, yaffs_obj_t *obj)
{

	struct yaffs_allocator *allocator = dev->allocator;

	if(!allocator)
		YBUG();
	else {
		/* Link into the free list. */
		obj->siblings.next = (struct ylist_head *)(allocator->free_objs);
		allocator->free_objs = obj;
		allocator->n_free_objects++;
	}
}

void yaffs_deinit_raw_tnodes_and_objs(yaffs_dev_t *dev)
{
	if(dev->allocator){
		yaffs_deinit_raw_tnodes(dev);
		yaffs_deinit_raw_objs(dev);

		YFREE(dev->allocator);
		dev->allocator=NULL;
	} else
		YBUG();
}

void yaffs_init_raw_tnodes_and_objs(yaffs_dev_t *dev)
{
	struct yaffs_allocator *allocator;

	if(!dev->allocator){
		allocator = YMALLOC(sizeof(struct yaffs_allocator));
		if(allocator){
			dev->allocator = allocator;
			yaffs_init_raw_tnodes(dev);
			yaffs_init_raw_objs(dev);
		}
	} else
		YBUG();
}


#endif
