/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 * Nick Bane modifications flagged NCB
 * Endian handling patches by James Ng.
 * mkyaffs2image hacks by NCB
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * makeyaffs2image.c 
 *
 * Makes a YAFFS2 file system image that can be used to load up a file system.
 * Uses default Linux MTD layout - search for "NAND LAYOUT" to change.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "yaffs_ecc.h"
#include "yaffs_guts.h"

#include "yaffs_tagsvalidity.h"
#include "yaffs_packedtags2.h"

unsigned yaffs_trace_mask=0;

#define MAX_OBJECTS 10000

// Adjust these to match your NAND LAYOUT:
#define chunkSize 2048
#define spareSize 64
#define pagesPerBlock 64

const char * mkyaffsimage_c_version = "$Id: mkyaffs2image.c,v 1.5 2010-01-11 21:43:18 charles Exp $";


typedef struct
{
	dev_t dev;
	ino_t ino;
	int   obj;
} objItem;


static objItem obj_list[MAX_OBJECTS];
static int n_obj = 0;
static int obj_id = YAFFS_NOBJECT_BUCKETS + 1;

static int n_obj, nDirectories, nPages;

static int outFile;

static int error;
static int savedErrno;

static int convert_endian = 0;

static void fatal(const char *fn)
{
	perror(fn);
	error |= 1;
	exit(error);
}

static int warn(const char *fn)
{
	savedErrno = errno;
	perror(fn);
	error |= 2;
	return error;
}

static int obj_compare(const void *a, const void * b)
{
  objItem *oa, *ob;
  
  oa = (objItem *)a;
  ob = (objItem *)b;
  
  if(oa->dev < ob->dev) return -1;
  if(oa->dev > ob->dev) return 1;
  if(oa->ino < ob->ino) return -1;
  if(oa->ino > ob->ino) return 1;
  
  return 0;
}


static void add_obj_to_list(dev_t dev, ino_t ino, int obj)
{
	if(n_obj < MAX_OBJECTS)
	{
		obj_list[n_obj].dev = dev;
		obj_list[n_obj].ino = ino;
		obj_list[n_obj].obj = obj;
		n_obj++;
		qsort(obj_list,n_obj,sizeof(objItem),obj_compare);
		
	}
	else
	{
		// oops! not enough space in the object array
		fprintf(stderr,"Not enough space in object array\n");
		exit(1);
	}
}


static int find_obj_in_list(dev_t dev, ino_t ino)
{
	objItem *i = NULL;
	objItem test;

	test.dev = dev;
	test.ino = ino;
	
	if(n_obj > 0)
	{
		i = bsearch(&test,obj_list,n_obj,sizeof(objItem),obj_compare);
	}

	if(i)
	{
		return i->obj;
	}
	return -1;
}

/* This little function converts a little endian tag to a big endian tag.
 * NOTE: The tag is not usable after this other than calculating the CRC
 * with.
 */
static void little_to_big_endian(yaffs_tags_t *tagsPtr)
{
#if 0 // FIXME NCB
    yaffs_tags_union_t * tags = (yaffs_tags_union_t* )tagsPtr; // Work in bytes.
    yaffs_tags_union_t   temp;

    memset(&temp, 0, sizeof(temp));
    // Ick, I hate magic numbers.
    temp.as_bytes[0] = ((tags->as_bytes[2] & 0x0F) << 4) | ((tags->as_bytes[1] & 0xF0) >> 4);
    temp.as_bytes[1] = ((tags->as_bytes[1] & 0x0F) << 4) | ((tags->as_bytes[0] & 0xF0) >> 4);
    temp.as_bytes[2] = ((tags->as_bytes[0] & 0x0F) << 4) | ((tags->as_bytes[2] & 0x30) >> 2) | ((tags->as_bytes[3] & 0xC0) >> 6);
    temp.as_bytes[3] = ((tags->as_bytes[3] & 0x3F) << 2) | ((tags->as_bytes[2] & 0xC0) >> 6);
    temp.as_bytes[4] = ((tags->as_bytes[6] & 0x03) << 6) | ((tags->as_bytes[5] & 0xFC) >> 2);
    temp.as_bytes[5] = ((tags->as_bytes[5] & 0x03) << 6) | ((tags->as_bytes[4] & 0xFC) >> 2);
    temp.as_bytes[6] = ((tags->as_bytes[4] & 0x03) << 6) | (tags->as_bytes[7] & 0x3F);
    temp.as_bytes[7] = (tags->as_bytes[6] & 0xFC) | ((tags->as_bytes[7] & 0xC0) >> 6);

    // Now copy it back.
    tags->as_bytes[0] = temp.as_bytes[0];
    tags->as_bytes[1] = temp.as_bytes[1];
    tags->as_bytes[2] = temp.as_bytes[2];
    tags->as_bytes[3] = temp.as_bytes[3];
    tags->as_bytes[4] = temp.as_bytes[4];
    tags->as_bytes[5] = temp.as_bytes[5];
    tags->as_bytes[6] = temp.as_bytes[6];
    tags->as_bytes[7] = temp.as_bytes[7];
#endif
}

static void shuffle_oob(char *spareData, yaffs_PackedTags2 *pt)
{
	assert(sizeof(*pt) <= spareSize);
	// NAND LAYOUT: For non-trivial OOB orderings, here would be a good place to shuffle.
	memcpy(spareData, pt, sizeof(*pt));
}

static int write_chunk(__u8 *data, __u32 obj_id, __u32 chunk_id, __u32 n_bytes)
{
	yaffs_ext_tags t;
	yaffs_PackedTags2 pt;
	char spareData[spareSize];

	if (write(outFile,data,chunkSize) != chunkSize)
		fatal("write");

	yaffs_init_tags(&t);
	
	t.chunk_id = chunk_id;
//	t.serial_number = 0;
	t.serial_number = 1;	// **CHECK**
	t.n_bytes = n_bytes;
	t.obj_id = obj_id;
	
	t.seq_number = YAFFS_LOWEST_SEQUENCE_NUMBER;

// added NCB **CHECK**
	t.chunk_used = 1;

	if (convert_endian)
	{
    	    little_to_big_endian(&t);
	}

	nPages++;

	memset(&pt, 0, sizeof(pt));
	yaffs_PackTags2(&pt,&t,1);

	memset(spareData, 0xff, sizeof(spareData));
	shuffle_oob(spareData, &pt);

	if (write(outFile,spareData,sizeof(spareData)) != sizeof(spareData))
		fatal("write");
	return 0;
}

#define SWAP32(x)   ((((x) & 0x000000FF) << 24) | \
                     (((x) & 0x0000FF00) << 8 ) | \
                     (((x) & 0x00FF0000) >> 8 ) | \
                     (((x) & 0xFF000000) >> 24))

#define SWAP16(x)   ((((x) & 0x00FF) << 8) | \
                     (((x) & 0xFF00) >> 8))
        
// This one is easier, since the types are more standard. No funky shifts here.
static void object_header_little_to_big_endian(yaffs_obj_header* oh)
{
    int i;    
    oh->type = SWAP32(oh->type); // GCC makes enums 32 bits.
    oh->parent_obj_id = SWAP32(oh->parent_obj_id); // int
    oh->sum_no_longer_used = SWAP16(oh->sum_no_longer_used); // __u16 - Not used, but done for completeness.
    // name = skip. Char array. Not swapped.
    oh->yst_mode = SWAP32(oh->yst_mode);
#ifdef CONFIG_YAFFS_WINCE // WinCE doesn't implement this, but we need to just in case. 
    // In fact, WinCE would be *THE* place where this would be an issue.
    // Why? WINCE is little-endian only.

    {
        int n = sizeof(oh->not_for_wince)/sizeof(oh->not_for_wince[0]);
        for(i = 0; i < n; i++)
            oh->not_for_wince[i] = SWAP32(oh->not_for_wince[i]);
    }
#else
    // Regular POSIX.
    oh->yst_uid = SWAP32(oh->yst_uid);
    oh->yst_gid = SWAP32(oh->yst_gid);
    oh->yst_atime = SWAP32(oh->yst_atime);
    oh->yst_mtime = SWAP32(oh->yst_mtime);
    oh->yst_ctime = SWAP32(oh->yst_ctime);
#endif

    oh->file_size = SWAP32(oh->file_size); // Aiee. An int... signed, at that!
    oh->equiv_id = SWAP32(oh->equiv_id);
    // alias  - char array.
    oh->yst_rdev = SWAP32(oh->yst_rdev);

#ifdef CONFIG_YAFFS_WINCE
    oh->win_ctime[0] = SWAP32(oh->win_ctime[0]);
    oh->win_ctime[1] = SWAP32(oh->win_ctime[1]);
    oh->win_atime[0] = SWAP32(oh->win_atime[0]);
    oh->win_atime[1] = SWAP32(oh->win_atime[1]);
    oh->win_mtime[0] = SWAP32(oh->win_mtime[0]);
    oh->win_mtime[1] = SWAP32(oh->win_mtime[1]);
#else

    {
        int n = sizeof(oh->room_to_grow)/sizeof(oh->room_to_grow[0]);
        for(i=0; i < n; i++)
            oh->room_to_grow[i] = SWAP32(oh->room_to_grow[i]);
    }
#endif
}

static int write_object_header(int obj_id, yaffs_obj_type t, struct stat *s, int parent, const char *name, int equivalentObj, const char * alias)
{
	__u8 bytes[chunkSize];
	
	
	yaffs_obj_header *oh = (yaffs_obj_header *)bytes;
	
	memset(bytes,0xff,sizeof(bytes));
	
	oh->type = t;

	oh->parent_obj_id = parent;
	
	if (strlen(name)+1 > sizeof(oh->name))
	{
		errno = ENAMETOOLONG;
		return warn("object name");
	}
	memset(oh->name,0,sizeof(oh->name));
	strcpy(oh->name,name);
	
	
	if(t != YAFFS_OBJECT_TYPE_HARDLINK)
	{
		oh->yst_mode = s->st_mode;
		oh->yst_uid = s->st_uid;
// NCB 12/9/02		oh->yst_gid = s->yst_uid;
		oh->yst_gid = s->st_gid;
		oh->yst_atime = s->st_atime;
		oh->yst_mtime = s->st_mtime;
		oh->yst_ctime = s->st_ctime;
		oh->yst_rdev  = s->st_rdev;
	}
	
	if(t == YAFFS_OBJECT_TYPE_FILE)
	{
		oh->file_size = s->st_size;
	}
	
	if(t == YAFFS_OBJECT_TYPE_HARDLINK)
	{
		oh->equiv_id = equivalentObj;
	}
	
	if(t == YAFFS_OBJECT_TYPE_SYMLINK)
	{
		if (strlen(alias)+1 > sizeof(oh->alias))
		{
			errno = ENAMETOOLONG;
			return warn("object alias");
		}
		memset(oh->alias,0,sizeof(oh->alias));
		strcpy(oh->alias,alias);
	}

	if (convert_endian)
	{
    		object_header_little_to_big_endian(oh);
	}
	
	return write_chunk(bytes,obj_id,0,0xffff);
	
}

static void pad_image()
{
	__u8 data[chunkSize + spareSize];
	int padPages = (nPages % pagesPerBlock);

	if (padPages)
	{
		memset(data, 0xff, sizeof(data));
		for (padPages = pagesPerBlock-padPages; padPages; padPages--)
		{
			if (write(outFile, data, sizeof(data)) != sizeof(data))
				fatal("write");
		}
	}
}

static int process_directory(int parent, const char *path)
{

	DIR *dir;
	struct dirent *entry;

	nDirectories++;
	
	dir = opendir(path);
	if(!dir)
	{
		warn("opendir");
	}
	else
	{
		while((entry = readdir(dir)) != NULL)
		{
		
			/* Ignore . and .. */
			if(strcmp(entry->d_name,".") &&
			   strcmp(entry->d_name,".."))
 			{
 				char full_name[500];
				struct stat stats;
				int equivalentObj;
				int newObj;
				
				if (snprintf(full_name,sizeof(full_name),"%s/%s",path,entry->d_name) >= (int)sizeof(full_name))
				{
					error = -1;
					continue;
				}
				
				if (lstat(full_name,&stats) < 0)
				{
					warn("lstat");
					continue;
				}
				
				if(S_ISLNK(stats.st_mode) ||
				    S_ISREG(stats.st_mode) ||
				    S_ISDIR(stats.st_mode) ||
				    S_ISFIFO(stats.st_mode) ||
				    S_ISBLK(stats.st_mode) ||
				    S_ISCHR(stats.st_mode) ||
				    S_ISSOCK(stats.st_mode))
				{
				
					newObj = obj_id++;
					n_obj++;
					
					printf("Object %d, %s is a ",newObj,full_name);
					
					/* We're going to create an object for it */
					if((equivalentObj = find_obj_in_list(stats.st_dev, stats.st_ino)) > 0)
					{
					 	/* we need to make a hard link */
					 	printf("hard link to object %d\n",equivalentObj);
						write_object_header(newObj, YAFFS_OBJECT_TYPE_HARDLINK, &stats, parent, entry->d_name, equivalentObj, NULL);
					}
					else 
					{
						
						add_obj_to_list(stats.st_dev,stats.st_ino,newObj);
						
						if(S_ISLNK(stats.st_mode))
						{
					
							char symname[500];
						
							memset(symname,0, sizeof(symname));
					
							if (readlink(full_name,symname,sizeof(symname) -1) < 0)
							{
								warn("readlink");
							}
							else
							{
								printf("symlink to \"%s\"\n",symname);
								write_object_header(newObj, YAFFS_OBJECT_TYPE_SYMLINK, &stats, parent, entry->d_name, -1, symname);
							}
						}
						else if(S_ISREG(stats.st_mode))
						{
							printf("file, ");
							if(write_object_header(newObj, YAFFS_OBJECT_TYPE_FILE, &stats, parent, entry->d_name, -1, NULL) == 0)
							{
								int h;
								__u8 bytes[chunkSize];
								int n_bytes;
								int chunk = 0;
								
								h = open(full_name,O_RDONLY);
								if(h >= 0)
								{
									memset(bytes,0xff,sizeof(bytes));
									while((n_bytes = read(h,bytes,sizeof(bytes))) > 0)
									{
										chunk++;
										write_chunk(bytes,newObj,chunk,n_bytes);
										memset(bytes,0xff,sizeof(bytes));
									}
									if(n_bytes < 0) 
									   warn("read");
									   
									printf("%d data chunks written\n",chunk);
									close(h);
								}
								else
								{
									warn("open");
								}
								
							}							
														
						}
						else if(S_ISSOCK(stats.st_mode))
						{
							printf("socket\n");
							write_object_header(newObj, YAFFS_OBJECT_TYPE_SPECIAL, &stats, parent, entry->d_name, -1, NULL);
						}
						else if(S_ISFIFO(stats.st_mode))
						{
							printf("fifo\n");
							write_object_header(newObj, YAFFS_OBJECT_TYPE_SPECIAL, &stats, parent, entry->d_name, -1, NULL);
						}
						else if(S_ISCHR(stats.st_mode))
						{
							printf("character device\n");
							write_object_header(newObj, YAFFS_OBJECT_TYPE_SPECIAL, &stats, parent, entry->d_name, -1, NULL);
						}
						else if(S_ISBLK(stats.st_mode))
						{
							printf("block device\n");
							write_object_header(newObj, YAFFS_OBJECT_TYPE_SPECIAL, &stats, parent, entry->d_name, -1, NULL);
						}
						else if(S_ISDIR(stats.st_mode))
						{
							printf("directory\n");
							if (write_object_header(newObj, YAFFS_OBJECT_TYPE_DIRECTORY, &stats, parent, entry->d_name, -1, NULL) == 0)
								process_directory(newObj,full_name);
						}
					}
				}
				else
				{
					fprintf(stderr, "%s: unhandled type\n", full_name);
					error |= 2;
					savedErrno = EINVAL;
				}
			}
		}
		closedir(dir);
	}
	
	return 0;

}


int main(int argc, char *argv[])
{
	struct stat stats;
	
	printf("mkyaffs2image: image building tool for YAFFS2 built "__DATE__"\n");
	
	if(argc < 3)
	{
		printf("usage: mkyaffs2image dir image_file [convert]\n");
		printf("           dir        the directory tree to be converted\n");
		printf("           image_file the output file to hold the image\n");
        printf("           'convert'  produce a big-endian image from a little-endian machine\n");
		exit(1);
	}

    if ((argc == 4) && (!strncmp(argv[3], "convert", strlen("convert"))))
    {
        convert_endian = 1;
    }
    
	if(stat(argv[1],&stats) < 0)
	{
		printf("Could not stat %s\n",argv[1]);
		exit(1);
	}
	
	if(!S_ISDIR(stats.st_mode))
	{
		printf(" %s is not a directory\n",argv[1]);
		exit(1);
	}
	
	outFile = open(argv[2],O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE);
	
	
	if(outFile < 0)
	{
		printf("Could not open output file %s\n",argv[2]);
		exit(1);
	}
	
	printf("Processing directory %s into image file %s\n",argv[1],argv[2]);
	process_directory(YAFFS_OBJECTID_ROOT,argv[1]);
	
	pad_image();

	close(outFile);
	
	if(error)
	{
		errno = savedErrno;
		perror("operation incomplete");
	}
	else
	{
		printf("Operation complete.\n"
		       "%d objects in %d directories\n"
		       "%d NAND pages\n",n_obj, nDirectories, nPages);
	}
	
	exit(error);
}	

