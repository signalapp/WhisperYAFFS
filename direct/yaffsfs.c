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

#include "yaffsfs.h"
#include "yaffs_guts.h"
#include "yaffscfg.h"
#include "yportenv.h"
#include "yaffs_trace.h"

#include <string.h> /* for memset */

#define YAFFSFS_MAX_SYMLINK_DEREFERENCES 5

#ifndef NULL
#define NULL ((void *)0)
#endif


/* YAFFSFS_RW_SIZE must be a power of 2 */
#define YAFFSFS_RW_SHIFT (13)
#define YAFFSFS_RW_SIZE  (1<<YAFFSFS_RW_SHIFT)

/* Some forward references */
static yaffs_obj_t *yaffsfs_FindObject(yaffs_obj_t *relativeDirectory, const YCHAR *path, int symDepth);
static void yaffsfs_RemoveObjectCallback(yaffs_obj_t *obj);

unsigned int yaffs_wr_attempts;

/*
 * Handle management.
 * There are open inodes in yaffsfs_Inode.
 * There are open handles in yaffsfs_Handle.
 *
 * Things are structured this way to be like the Linux VFS model
 * so that interactions with the yaffs guts calls are similar.
 * That means more common code paths and less special code.
 * That means better testing etc.
 */

typedef struct {
	int count;	/* Number of handles accessing this inode */
	yaffs_obj_t *iObj;
} yaffsfs_Inode;

typedef struct{
	__u8 	reading:1;
	__u8 	writing:1;
	__u8 	append:1;
	__u8	shareRead:1;
	__u8	shareWrite:1;
	int	inodeId:12;	/* Index to corresponding yaffsfs_Inode */
	int	useCount:10;	/* Use count for this handle */
	__u32 position;		/* current position in file */
}yaffsfs_Handle;

static yaffsfs_Inode yaffsfs_inode[YAFFSFS_N_HANDLES];
static yaffsfs_Handle yaffsfs_handle[YAFFSFS_N_HANDLES];
static int yaffsfs_handlesInitialised;

/*
 * yaffsfs_InitHandle
 * Inilitalise handle management on start-up.
 */

static void yaffsfs_InitHandles(void)
{
	int i;
	if(yaffsfs_handlesInitialised)
                return;

	memset(yaffsfs_inode,0,sizeof(yaffsfs_inode));
	memset(yaffsfs_handle,0,sizeof(yaffsfs_handle));
	for(i = 0; i < YAFFSFS_N_HANDLES; i++)
		yaffsfs_handle[i].inodeId = -1;
}

yaffsfs_Handle *yaffsfs_GetHandlePointer(int h)
{
	if(h < 0 || h >= YAFFSFS_N_HANDLES)
		return NULL;

	return &yaffsfs_handle[h];
}

yaffsfs_Inode *yaffsfs_GetInodePointer(int handle)
{
	yaffsfs_Handle *h = yaffsfs_GetHandlePointer(handle);

	if(h && h->useCount > 0 && h->inodeId >= 0 && h->inodeId < YAFFSFS_N_HANDLES)
		return  &yaffsfs_inode[h->inodeId];

	return NULL;
}

yaffs_obj_t *yaffsfs_GetHandleObject(int handle)
{
	yaffsfs_Inode *in = yaffsfs_GetInodePointer(handle);

	if(in)
		return in->iObj;

	return NULL;
}

/*
 * yaffsfs_FindInodeIdForObject
 * Find the inode entry for an object, if it exists.
 */

static int yaffsfs_FindInodeIdForObject(yaffs_obj_t *obj)
{
	int i;
	int ret = -1;
	
	if(obj)
		obj = yaffs_get_equivalent_obj(obj);

	/* Look for it in open inode table*/
	for(i = 0; i < YAFFSFS_N_HANDLES && ret < 0; i++){
		if(yaffsfs_inode[i].iObj == obj)
			ret = i;
	}
	return ret;
}

/*
 * yaffsfs_GetInodeIdForObject
 * Grab an inode entry when opening a new inode.
 */
static int yaffsfs_GetInodeIdForObject(yaffs_obj_t *obj)
{
	int i;
	int ret;
	yaffsfs_Inode *in = NULL;
	
	if(obj)
		obj = yaffs_get_equivalent_obj(obj);

        ret = yaffsfs_FindInodeIdForObject(obj);

	for(i = 0; i < YAFFSFS_N_HANDLES && ret < 0; i++){
		if(!yaffsfs_inode[i].iObj)
			ret = i;
	}

	if(ret>=0){
		in = &yaffsfs_inode[ret];
		if(!in->iObj)
			in->count = 0;
		in->iObj = obj;
		in->count++;
	}
	
	
	return ret;
}


static int yaffsfs_CountHandles(yaffs_obj_t *obj)
{
	int i = yaffsfs_FindInodeIdForObject(obj);

	if(i >= 0)
		return yaffsfs_inode[i].count;
	else
		return 0;
}

static void yaffsfs_ReleaseInode(yaffsfs_Inode *in)
{
	yaffs_obj_t *obj;
	
	obj = in->iObj;

	if(obj->unlinked)
		yaffs_del_obj(obj);
	
	obj->my_inode = NULL;
	in->iObj = NULL;

}

static void yaffsfs_PutInode(int inodeId)
{
	if(inodeId >= 0 && inodeId < YAFFSFS_N_HANDLES){
		yaffsfs_Inode *in = & yaffsfs_inode[inodeId];
		in->count--;
		if(in->count <= 0)
			yaffsfs_ReleaseInode(in);
	}	
}


/*
 * yaffsfs_GetHandle
 * Grab a handle (when opening a file)
 */

static int yaffsfs_GetNewHandle(void)
{
	int i;
	yaffsfs_Handle *h;

	for(i = 0; i < YAFFSFS_N_HANDLES; i++){
		h = yaffsfs_GetHandlePointer(i);
		if(!h){
			/* todo bug: should never happen */
		}
		if(h->useCount < 1){
			memset(h,0,sizeof(yaffsfs_Handle));
			h->inodeId=-1;
			h->useCount=1;
			return i;
		}
	}
	return -1;
}

/*
 * yaffs_get_handle
 * Increase use of handle when reading/writing a file
 */
static int yaffsfs_GetHandle(int handle)
{
	yaffsfs_Handle *h = yaffsfs_GetHandlePointer(handle);

	if(h && h->useCount > 0){	
		h->useCount++;
	}
	return 0;
}

/*
 * yaffs_put_handle
 * Let go of a handle when closing a file or aborting an open or
 * ending a read or write.
 */
static int yaffsfs_PutHandle(int handle)
{
	yaffsfs_Handle *h = yaffsfs_GetHandlePointer(handle);

	if(h && h->useCount > 0){	
		h->useCount--;
		if(h->useCount < 1){
			if(h->inodeId >= 0){
				yaffsfs_PutInode(h->inodeId);
				h->inodeId = -1;
			}
		}
	}
	return 0;
}




/*
 *  Stuff to search for a directory from a path
 */


int yaffsfs_Match(YCHAR a, YCHAR b)
{
	/* case sensitive */
	return (a == b);
}

int yaffsfs_IsPathDivider(YCHAR ch)
{
	const YCHAR *str = YAFFS_PATH_DIVIDERS;

	while(*str){
		if(*str == ch)
			return 1;
		str++;
	}

	return 0;
}



YLIST_HEAD(yaffsfs_deviceList);

/*
 * yaffsfs_FindDevice
 * yaffsfs_FindRoot
 * Scan the configuration list to find the root.
 * Curveballs: Should match paths that end in '/' too
 * Curveball2 Might have "/x/ and "/x/y". Need to return the longest match
 */
static yaffs_dev_t *yaffsfs_FindDevice(const YCHAR *path, YCHAR **restOfPath)
{
	struct ylist_head *cfg;
	const YCHAR *leftOver;
	const YCHAR *p;
	yaffs_dev_t *retval = NULL;
	yaffs_dev_t *dev = NULL;
	int thisMatchLength;
	int longestMatch = -1;
	int matching;

	/*
	 * Check all configs, choose the one that:
	 * 1) Actually matches a prefix (ie /a amd /abc will not match
	 * 2) Matches the longest.
	 */
	ylist_for_each(cfg, &yaffsfs_deviceList){
		dev = ylist_entry(cfg, yaffs_dev_t, dev_list);
		leftOver = path;
		p = dev->param.name;
		thisMatchLength = 0;
		matching = 1;


		while(matching && *p && *leftOver){
			/* Skip over any /s */
			while(yaffsfs_IsPathDivider(*p))
			      p++;

			/* Skip over any /s */
			while(yaffsfs_IsPathDivider(*leftOver))
		              leftOver++;

			/* Now match the text part */
		        while(matching &&
		              *p && !yaffsfs_IsPathDivider(*p) &&
		              *leftOver && !yaffsfs_IsPathDivider(*leftOver)){
			      	if(yaffsfs_Match(*p,*leftOver)){
			      		p++;
			      		leftOver++;
			      		thisMatchLength++;
				} else {
					matching = 0;
				}
			}
		}

		/* Skip over any /s in leftOver */
		while(yaffsfs_IsPathDivider(*leftOver))
	              leftOver++;

		// Skip over any /s in p
		while(yaffsfs_IsPathDivider(*p))
	              p++;

		// p should now be at the end of the string (ie. fully matched)
		if(*p)
			matching = 0;

		if( matching && (thisMatchLength > longestMatch))
		{
			// Matched prefix
			*restOfPath = (YCHAR *)leftOver;
			retval = dev;
			longestMatch = thisMatchLength;
		}

	}
	return retval;
}

#if 0
static yaffs_dev_t *yaffsfs_FindDevice(const YCHAR *path, YCHAR **restOfPath)
{
	yaffsfs_DeviceConfiguration *cfg = yaffsfs_configurationList;
	const YCHAR *leftOver;
	const YCHAR *p;
	yaffs_dev_t *retval = NULL;
	int thisMatchLength;
	int longestMatch = -1;

	/*
	 * Check all configs, choose the one that:
	 * 1) Actually matches a prefix (ie /a amd /abc will not match
	 * 2) Matches the longest.
	 */
	while(cfg && cfg->prefix && cfg->dev){
		leftOver = path;
		p = cfg->prefix;
		thisMatchLength = 0;

		while(*p &&  /* unmatched part of prefix */
		      !(yaffsfs_IsPathDivider(*p) && (p[1] == 0)) &&
		      *leftOver && yaffsfs_Match(*p,*leftOver)){
			p++;
			leftOver++;
			thisMatchLength++;
		}


		if((!*p || (yaffsfs_IsPathDivider(*p) && (p[1] == 0))) &&  /* end of prefix */
		   (!*leftOver || yaffsfs_IsPathDivider(*leftOver)) && /* no more in this path name part */
		   (thisMatchLength > longestMatch)){
			/* Matched prefix */
			*restOfPath = (YCHAR *)leftOver;
			retval = cfg->dev;
			longestMatch = thisMatchLength;
		}
		cfg++;
	}
	return retval;
}
#endif

static yaffs_obj_t *yaffsfs_FindRoot(const YCHAR *path, YCHAR **restOfPath)
{

	yaffs_dev_t *dev;

	dev= yaffsfs_FindDevice(path,restOfPath);
	if(dev && dev->is_mounted){
		return dev->root_dir;
	}
	return NULL;
}

static yaffs_obj_t *yaffsfs_FollowLink(yaffs_obj_t *obj,int symDepth)
{

	while(obj && obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK){
		YCHAR *alias = obj->variant.symlink_variant.alias;

		if(yaffsfs_IsPathDivider(*alias))
			/* Starts with a /, need to scan from root up */
			obj = yaffsfs_FindObject(NULL,alias,symDepth++);
		else
			/* Relative to here, so use the parent of the symlink as a start */
			obj = yaffsfs_FindObject(obj->parent,alias,symDepth++);
	}
	return obj;
}


/*
 * yaffsfs_FindDirectory
 * Parse a path to determine the directory and the name within the directory.
 *
 * eg. "/data/xx/ff" --> puts name="ff" and returns the directory "/data/xx"
 */
static yaffs_obj_t *yaffsfs_DoFindDirectory(yaffs_obj_t *startDir,
				const YCHAR *path, YCHAR **name, int symDepth)
{
	yaffs_obj_t *dir;
	YCHAR *restOfPath;
	YCHAR str[YAFFS_MAX_NAME_LENGTH+1];
	int i;

	if(symDepth > YAFFSFS_MAX_SYMLINK_DEREFERENCES)
		return NULL;

	if(startDir){
		dir = startDir;
		restOfPath = (YCHAR *)path;
	}
	else
		dir = yaffsfs_FindRoot(path,&restOfPath);

	while(dir){
		/*
		 * parse off /.
		 * curve ball: also throw away surplus '/'
		 * eg. "/ram/x////ff" gets treated the same as "/ram/x/ff"
		 */
		while(yaffsfs_IsPathDivider(*restOfPath))
			restOfPath++; /* get rid of '/' */

		*name = restOfPath;
		i = 0;

		while(*restOfPath && !yaffsfs_IsPathDivider(*restOfPath)){
			if (i < YAFFS_MAX_NAME_LENGTH){
				str[i] = *restOfPath;
				str[i+1] = '\0';
				i++;
			}
			restOfPath++;
		}

		if(!*restOfPath)
			/* got to the end of the string */
			return dir;
		else{
			if(yaffs_strcmp(str,_Y(".")) == 0)
			{
				/* Do nothing */
			}
			else if(yaffs_strcmp(str,_Y("..")) == 0)
				dir = dir->parent;
			else{
				dir = yaffs_find_by_name(dir,str);

				while(dir && dir->variant_type == YAFFS_OBJECT_TYPE_SYMLINK)
					dir = yaffsfs_FollowLink(dir,symDepth);


				if(dir && dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
					dir = NULL;
			}
		}
	}
	/* directory did not exist. */
	return NULL;
}

static yaffs_obj_t *yaffsfs_FindDirectory(yaffs_obj_t *relativeDirectory,
					const YCHAR *path,YCHAR **name,int symDepth)
{
	return yaffsfs_DoFindDirectory(relativeDirectory,path,name,symDepth);
}

/*
 * yaffsfs_FindObject turns a path for an existing object into the object
 */
static yaffs_obj_t *yaffsfs_FindObject(yaffs_obj_t *relativeDirectory, const YCHAR *path,int symDepth)
{
	yaffs_obj_t *dir;
	YCHAR *name;

	dir = yaffsfs_FindDirectory(relativeDirectory,path,&name,symDepth);

	if(dir && *name)
		return yaffs_find_by_name(dir,name);

	return dir;
}


int yaffs_dup(int fd)
{
	int newHandle = -1;
	yaffsfs_Handle *oldPtr = NULL;
	yaffsfs_Handle *newPtr = NULL;

	yaffsfs_Lock();

	oldPtr = yaffsfs_GetHandlePointer(fd);
	if(oldPtr && oldPtr->useCount > 0)
		newHandle = yaffsfs_GetNewHandle();
	if(newHandle >= 0)
		newPtr = yaffsfs_GetHandlePointer(newHandle);

	if(newPtr){
		*newPtr = *oldPtr;
		return newHandle;
	}

	if(!oldPtr)
		yaffsfs_SetError(-EBADF);
	else
		yaffsfs_SetError(-ENOMEM);

	return -1;

}

int yaffs_open_sharing(const YCHAR *path, int oflag, int mode, int sharing)
{
	yaffs_obj_t *obj = NULL;
	yaffs_obj_t *dir = NULL;
	YCHAR *name;
	int handle = -1;
	yaffsfs_Handle *h = NULL;
	int openDenied = 0;
	int symDepth = 0;
	int errorReported = 0;
	__u8 shareRead = (sharing & YAFFS_SHARE_READ) ?  1 : 0;
	__u8 shareWrite = (sharing & YAFFS_SHARE_WRITE) ? 1 : 0;
	__u8 sharedReadAllowed;
	__u8 sharedWriteAllowed;
	__u8 alreadyReading;
	__u8 alreadyWriting;
	__u8 readRequested;
	__u8 writeRequested;

	/* O_EXCL only has meaning if O_CREAT is specified */
	if(!(oflag & O_CREAT))
		oflag &= ~(O_EXCL);

	/* O_TRUNC has no meaning if (O_CREAT | O_EXCL) is specified */
	if( (oflag & O_CREAT) & (oflag & O_EXCL))
		oflag &= ~(O_TRUNC);

	/* Todo: Are there any more flag combos to sanitise ? */


	yaffsfs_Lock();

	handle = yaffsfs_GetNewHandle();

	if(handle >= 0){

		h = yaffsfs_GetHandlePointer(handle);

		/* try to find the exisiting object */
		obj = yaffsfs_FindObject(NULL,path,0);

		if(obj && obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK)
			obj = yaffsfs_FollowLink(obj,symDepth++);

		if(obj)
			obj = yaffs_get_equivalent_obj(obj);

		if(obj &&
			obj->variant_type != YAFFS_OBJECT_TYPE_FILE &&
			obj->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
			obj = NULL;

		if(obj){

			/* The file already exists or it might be a directory */

			/* If it is a directory then we can't open it as a file */
			if(obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY){
				openDenied = 1;
				yaffsfs_SetError(-EISDIR);
				errorReported = 1;
			}

			/* Open should fail if O_CREAT and O_EXCL are specified since
			 * the file exists
			 */
			if((oflag & O_EXCL) && (oflag & O_CREAT)){
				openDenied = 1;
				yaffsfs_SetError(-EEXIST);
				errorReported = 1;
			}

			/* Check file permissions */
			if( (oflag & (O_RDWR | O_WRONLY)) == 0 &&     /* ie O_RDONLY */
			   !(obj->yst_mode & S_IREAD))
				openDenied = 1;

			if( (oflag & O_RDWR) &&
			   !(obj->yst_mode & S_IREAD))
				openDenied = 1;

			if( (oflag & (O_RDWR | O_WRONLY)) &&
			   !(obj->yst_mode & S_IWRITE))
				openDenied = 1;

			/* Check sharing of an existing object. */
			{
				yaffsfs_Handle *h;
				int i;
				sharedReadAllowed = 1;
				sharedWriteAllowed = 1;
				alreadyReading = 0;
				alreadyWriting = 0;
				for( i = 0; i < YAFFSFS_N_HANDLES; i++){
					h = &yaffsfs_handle[i];
					if(h->useCount > 0 &&
						h->inodeId >= 0 &&
						yaffsfs_inode[h->inodeId].iObj == obj){
						if(!h->shareRead)
							sharedReadAllowed = 0;
						if(!h->shareWrite)
							sharedWriteAllowed = 0;
						if(h->reading)
							alreadyReading = 1;
						if(h->writing)
							alreadyWriting = 0;
					}
				}

				readRequested = (oflag & (O_RDWR | O_RDONLY)) ? 1 : 0;
				writeRequested = (oflag & (O_RDWR | O_WRONLY)) ? 1 : 0;

				if((!sharedReadAllowed && readRequested)|| 
					(!shareRead  && alreadyReading) ||
					(!sharedWriteAllowed && writeRequested) ||
					(!shareWrite && alreadyWriting)){
					openDenied = 1;
					yaffsfs_SetError(-EBUSY);
					errorReported=1;
				}
			}

		} else if((oflag & O_CREAT)) {
			/* Let's see if we can create this file */
			dir = yaffsfs_FindDirectory(NULL,path,&name,0);
			if(dir  && dir->my_dev->read_only){
				yaffsfs_SetError(-EINVAL);
				errorReported = 1;
			} else if(dir)
				obj = yaffs_create_file(dir,name,mode,0,0);
			else {
				yaffsfs_SetError(-ENOTDIR);
				errorReported = 1;
			}
		}

		if(obj && !openDenied) {
			int inodeId = yaffsfs_GetInodeIdForObject(obj);

			if(inodeId<0) {
				/*
				 * Todo: Fix any problem if inodes run out, though that
				 * can't happen if the number of inode items >= number of handles. 
				 */
			}
			
			h->inodeId = inodeId;
			h->reading = (oflag & (O_RDONLY | O_RDWR)) ? 1 : 0;
			h->writing = (oflag & (O_WRONLY | O_RDWR)) ? 1 : 0;
			h->append =  (oflag & O_APPEND) ? 1 : 0;
			h->position = 0;
			h->shareRead = shareRead;
			h->shareWrite = shareWrite;

			/* Hook inode to object */
                        obj->my_inode = (void*) &yaffsfs_inode[inodeId];

                        if((oflag & O_TRUNC) && h->writing)
                                yaffs_resize_file(obj,0);
		} else {
			yaffsfs_PutHandle(handle);
			if(!errorReported) {
				yaffsfs_SetError(-EACCES);
				errorReported = 1;
			}
			handle = -1;
		}
	}

	yaffsfs_Unlock();

	return handle;
}

int yaffs_open(const YCHAR *path, int oflag, int mode)
{
	return yaffs_open_sharing(path, oflag, mode, YAFFS_SHARE_READ | YAFFS_SHARE_WRITE);
}

int yaffs_Dofsync(int fd,int datasync)
{
	yaffsfs_Handle *h = NULL;
	int retVal = 0;

	yaffsfs_Lock();

	h = yaffsfs_GetHandlePointer(fd);

	if(h && h->useCount > 0)
		/* flush the file */
		yaffs_flush_file(yaffsfs_inode[h->inodeId].iObj,1,datasync);
	else {
		/* bad handle */
		yaffsfs_SetError(-EBADF);
		retVal = -1;
	}

	yaffsfs_Unlock();

	return retVal;
}

int yaffs_fsync(int fd)
{
	return yaffs_Dofsync(fd,0);
}

int yaffs_flush(int fd)
{
	return yaffs_fsync(fd);
}

int yaffs_fdatasync(int fd)
{
	return yaffs_Dofsync(fd,1);
}

int yaffs_close(int fd)
{
	yaffsfs_Handle *h = NULL;
	int retVal = 0;

	yaffsfs_Lock();

	h = yaffsfs_GetHandlePointer(fd);

	if(h && h->useCount > 0) {
		/* clean up */
		yaffs_flush_file(yaffsfs_inode[h->inodeId].iObj,1,0);
		yaffsfs_PutHandle(fd);
		retVal = 0;
	} else {
		/* bad handle */
		yaffsfs_SetError(-EBADF);
		retVal = -1;
	}

	yaffsfs_Unlock();

	return retVal;
}



int yaffsfs_do_read(int fd, void *vbuf, unsigned int nbyte, int isPread, int offset)
{
	yaffsfs_Handle *h = NULL;
	yaffs_obj_t *obj = NULL;
	int pos = 0;
	int startPos = 0;
	int nRead = 0;
	int nToRead = 0;
	int totalRead = 0;
	unsigned int maxRead;
	__u8 *buf = (__u8 *)vbuf;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj){
		/* bad handle */
		yaffsfs_SetError(-EBADF);
		totalRead = -1;
	} else if( h && obj){
		if(isPread)
			startPos = offset;
		else
			startPos = h->position;

		pos = startPos;
					
		if(yaffs_get_obj_length(obj) > pos)
			maxRead = yaffs_get_obj_length(obj) - pos;
		else
			maxRead = 0;

		if(nbyte > maxRead)
			nbyte = maxRead;


		yaffsfs_GetHandle(fd);

		while(nbyte > 0) {
			nToRead = YAFFSFS_RW_SIZE - (pos & (YAFFSFS_RW_SIZE -1));
			if(nToRead > nbyte)
				nToRead = nbyte;
			nRead = yaffs_file_rd(obj,buf,pos,nToRead);

			if(nRead > 0){
				totalRead += nRead;
				pos += nRead;
				buf += nRead;
			}

			if(nRead == nToRead)
				nbyte-=nRead;
			else
				nbyte = 0; /* no more to read */
					
					
			if(nbyte > 0){
				yaffsfs_Unlock();
				yaffsfs_Lock();
			}

		}

		yaffsfs_PutHandle(fd);

		if(!isPread) {
			if(totalRead >= 0)
				h->position = startPos + totalRead;
			else {
					/* todo error */
			}
		}

	}

	yaffsfs_Unlock();

	return (totalRead >= 0) ? totalRead : -1;

}

int yaffs_read(int fd, void *buf, unsigned int nbyte)
{
	return yaffsfs_do_read(fd, buf, nbyte, 0, 0);
}

int yaffs_pread(int fd, void *buf, unsigned int nbyte, unsigned int offset)
{
	return yaffsfs_do_read(fd, buf, nbyte, 1, offset);
}

int yaffsfs_do_write(int fd, const void *vbuf, unsigned int nbyte, int isPwrite, int offset)
{
	yaffsfs_Handle *h = NULL;
	yaffs_obj_t *obj = NULL;
	int pos = 0;
	int startPos = 0;
	int nWritten = 0;
	int totalWritten = 0;
	int write_trhrough = 0;
	int nToWrite = 0;
	const __u8 *buf = (const __u8 *)vbuf;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj){
		/* bad handle */
		yaffsfs_SetError(-EBADF);
		totalWritten = -1;
	} else if( h && obj && (!h->writing || obj->my_dev->read_only)){
		yaffsfs_SetError(-EINVAL);
		totalWritten=-1;
	} else if( h && obj){
		if(h->append)
			startPos = yaffs_get_obj_length(obj);
		else if(isPwrite)
			startPos = offset;
		else
			startPos = h->position;

		yaffsfs_GetHandle(fd);
		pos = startPos;
		while(nbyte > 0) {
			nToWrite = YAFFSFS_RW_SIZE - (pos & (YAFFSFS_RW_SIZE -1));
			if(nToWrite > nbyte)
				nToWrite = nbyte;

			nWritten = yaffs_wr_file(obj,buf,pos,nToWrite,write_trhrough);
			if(nWritten > 0){
				totalWritten += nWritten;
				pos += nWritten;
				buf += nWritten;
			}

			if(nWritten == nToWrite)
				nbyte -= nToWrite;
			else
				nbyte = 0;

			if(nWritten < 1 && totalWritten < 1){
				yaffsfs_SetError(-ENOSPC);
				totalWritten = -1;
			}

			if(nbyte > 0){
				yaffsfs_Unlock();
				yaffsfs_Lock();
			}
		}

		yaffsfs_PutHandle(fd);

		if(!isPwrite){
			if(totalWritten > 0)
				h->position = startPos + totalWritten;
			else {
				/* todo error */
			}
		}
	}

	yaffsfs_Unlock();

	return (totalWritten >= 0) ? totalWritten : -1;
}

int yaffs_write(int fd, const void *buf, unsigned int nbyte)
{
	return yaffsfs_do_write(fd, buf, nbyte, 0, 0);
}

int yaffs_pwrite(int fd, const void *buf, unsigned int nbyte, unsigned int offset)
{
	return yaffsfs_do_write(fd, buf, nbyte, 1, offset);
}


int yaffs_truncate(const YCHAR *path,off_t new_size)
{
	yaffs_obj_t *obj = NULL;
	int result = YAFFS_FAIL;

	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,path,0);
	if(obj)
		obj = yaffs_get_equivalent_obj(obj);

	if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->variant_type != YAFFS_OBJECT_TYPE_FILE)
		yaffsfs_SetError(-EISDIR);
	else if(obj->my_dev->read_only)
		yaffsfs_SetError(-EINVAL);
	else
		result = yaffs_resize_file(obj,new_size);

	yaffsfs_Unlock();


	return (result) ? 0 : -1;
}

int yaffs_ftruncate(int fd, off_t new_size)
{
	yaffsfs_Handle *h = NULL;
	yaffs_obj_t *obj = NULL;
	int result = 0;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj)
		/* bad handle */
		yaffsfs_SetError(-EBADF);
	else if(obj->my_dev->read_only)
		yaffsfs_SetError(-EINVAL);
	else
		/* resize the file */
		result = yaffs_resize_file(obj,new_size);
	yaffsfs_Unlock();


	return (result) ? 0 : -1;

}

off_t yaffs_lseek(int fd, off_t offset, int whence)
{
	yaffsfs_Handle *h = NULL;
	yaffs_obj_t *obj = NULL;
	int pos = -1;
	int fSize = -1;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj)
		/* bad handle */
		yaffsfs_SetError(-EBADF);
	else if(whence == SEEK_SET){
		if(offset >= 0)
			pos = offset;
	}
	else if(whence == SEEK_CUR) {
		if( (h->position + offset) >= 0)
			pos = (h->position + offset);
	}
	else if(whence == SEEK_END) {
		fSize = yaffs_get_obj_length(obj);
		if(fSize >= 0 && (fSize + offset) >= 0)
			pos = fSize + offset;
	}

	if(pos >= 0)
		h->position = pos;
	else {
		/* todo error */
	}


	yaffsfs_Unlock();

	return pos;
}


int yaffsfs_DoUnlink(const YCHAR *path,int isDirectory)
{
	yaffs_obj_t *dir = NULL;
	yaffs_obj_t *obj = NULL;
	YCHAR *name;
	int result = YAFFS_FAIL;

	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,path,0);
	dir = yaffsfs_FindDirectory(NULL,path,&name,0);
	if(!dir)
		yaffsfs_SetError(-ENOTDIR);
	else if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->my_dev->read_only)
		yaffsfs_SetError(-EINVAL);
	else if(!isDirectory && obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY)
		yaffsfs_SetError(-EISDIR);
	else if(isDirectory && obj->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
		yaffsfs_SetError(-ENOTDIR);
	else {
		result = yaffs_unlinker(dir,name);

		if(result == YAFFS_FAIL && isDirectory)
			yaffsfs_SetError(-ENOTEMPTY);
	}

	yaffsfs_Unlock();

	/* todo error */

	return (result == YAFFS_FAIL) ? -1 : 0;
}


int yaffs_rmdir(const YCHAR *path)
{
	return yaffsfs_DoUnlink(path,1);
}

int yaffs_unlink(const YCHAR *path)
{
	return yaffsfs_DoUnlink(path,0);
}

int yaffs_rename(const YCHAR *oldPath, const YCHAR *newPath)
{
	yaffs_obj_t *olddir = NULL;
	yaffs_obj_t *newdir = NULL;
	yaffs_obj_t *obj = NULL;
	YCHAR *oldname;
	YCHAR *newname;
	int result= YAFFS_FAIL;
	int rename_allowed = 1;

	yaffsfs_Lock();

	olddir = yaffsfs_FindDirectory(NULL,oldPath,&oldname,0);
	newdir = yaffsfs_FindDirectory(NULL,newPath,&newname,0);
	obj = yaffsfs_FindObject(NULL,oldPath,0);

	if(!olddir || !newdir || !obj) {
		/* bad file */
		yaffsfs_SetError(-EBADF);
		rename_allowed = 0;
	} else if(obj->my_dev->read_only){
		yaffsfs_SetError(-EINVAL);
		rename_allowed = 0;
	} else if(olddir->my_dev != newdir->my_dev) {
		/* oops must be on same device */
		/* todo error */
		yaffsfs_SetError(-EXDEV);
		rename_allowed = 0;
	} else if(obj && obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY) {
		/*
		 * It is a directory, check that it is not being renamed to
		 * being its own decendent.
		 * Do this by tracing from the new directory back to the root, checking for obj
		 */

		yaffs_obj_t *xx = newdir;

		while( rename_allowed && xx){
			if(xx == obj)
				rename_allowed = 0;
			xx = xx->parent;
		}
		if(!rename_allowed)
			yaffsfs_SetError(-EACCES);
	}

	if(rename_allowed)
		result = yaffs_rename_obj(olddir,oldname,newdir,newname);

	yaffsfs_Unlock();

	return (result == YAFFS_FAIL) ? -1 : 0;
}


static int yaffsfs_DoStat(yaffs_obj_t *obj,struct yaffs_stat *buf)
{
	int retVal = -1;

	if(obj)
		obj = yaffs_get_equivalent_obj(obj);

	if(obj && buf){
	    	buf->st_dev = (int)obj->my_dev->os_context;
	    	buf->st_ino = obj->obj_id;
	    	buf->st_mode = obj->yst_mode & ~S_IFMT; /* clear out file type bits */

	    	if(obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY)
			buf->st_mode |= S_IFDIR;
		else if(obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK)
			buf->st_mode |= S_IFLNK;
		else if(obj->variant_type == YAFFS_OBJECT_TYPE_FILE)
			buf->st_mode |= S_IFREG;

	    	buf->st_nlink = yaffs_get_obj_link_count(obj);
	    	buf->st_uid = 0;
	    	buf->st_gid = 0;;
	    	buf->st_rdev = obj->yst_rdev;
	    	buf->st_size = yaffs_get_obj_length(obj);
	    	buf->st_blksize = obj->my_dev->data_bytes_per_chunk;
	    	buf->st_blocks = (buf->st_size + buf->st_blksize -1)/buf->st_blksize;
#if CONFIG_YAFFS_WINCE
		buf->yst_wince_atime[0] = obj->win_atime[0];
		buf->yst_wince_atime[1] = obj->win_atime[1];
		buf->yst_wince_ctime[0] = obj->win_ctime[0];
		buf->yst_wince_ctime[1] = obj->win_ctime[1];
		buf->yst_wince_mtime[0] = obj->win_mtime[0];
		buf->yst_wince_mtime[1] = obj->win_mtime[1];
#else
    		buf->yst_atime = obj->yst_atime;
	    	buf->yst_ctime = obj->yst_ctime;
	    	buf->yst_mtime = obj->yst_mtime;
#endif
		retVal = 0;
	}
	return retVal;
}

static int yaffsfs_DoStatOrLStat(const YCHAR *path, struct yaffs_stat *buf,int doLStat)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(!doLStat && obj)
		obj = yaffsfs_FollowLink(obj,0);

	if(obj)
		retVal = yaffsfs_DoStat(obj,buf);
	else
		/* todo error not found */
		yaffsfs_SetError(-ENOENT);

	yaffsfs_Unlock();

	return retVal;

}

int yaffs_stat(const YCHAR *path, struct yaffs_stat *buf)
{
	return yaffsfs_DoStatOrLStat(path,buf,0);
}

int yaffs_lstat(const YCHAR *path, struct yaffs_stat *buf)
{
	return yaffsfs_DoStatOrLStat(path,buf,1);
}

int yaffs_fstat(int fd, struct yaffs_stat *buf)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj)
		retVal = yaffsfs_DoStat(obj,buf);
	else
		/* bad handle */
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

#ifndef CONFIG_YAFFS_WINCE
/* xattrib functions */


static int yaffs_do_setxattr(const YCHAR *path, const char *name, const void *data, int size, int flags, int follow)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(follow)
		obj = yaffsfs_FollowLink(obj,0);

	if(obj) {
		retVal = yaffs_set_xattrib(obj,name,data,size,flags);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* todo error not found */
		yaffsfs_SetError(-ENOENT);

	yaffsfs_Unlock();

	return retVal;

}

int yaffs_setxattr(const YCHAR *path, const char *name, const void *data, int size, int flags)
{
	return yaffs_do_setxattr(path, name, data, size, flags, 1);
}

int yaffs_lsetxattr(const YCHAR *path, const char *name, const void *data, int size, int flags)
{
	return yaffs_do_setxattr(path, name, data, size, flags, 0);
}



int yaffs_fsetxattr(int fd, const char *name, const void *data, int size, int flags)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj) {
		retVal = yaffs_set_xattrib(obj,name,data,size,flags);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* bad handle */
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

static int yaffs_do_getxattr(const YCHAR *path, const char *name, void *data, int size, int follow)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(follow)
		obj = yaffsfs_FollowLink(obj,0);

	if(obj) {
		retVal = yaffs_get_xattrib(obj,name,data,size);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* todo error not found */
		yaffsfs_SetError(-ENOENT);

	yaffsfs_Unlock();

	return retVal;

}

int yaffs_getxattr(const YCHAR *path, const char *name, void *data, int size)
{
	return yaffs_do_getxattr( path, name, data, size, 1);
}
int yaffs_lgetxattr(const YCHAR *path, const char *name, void *data, int size)
{
	return yaffs_do_getxattr( path, name, data, size, 0);
}



int yaffs_fgetxattr(int fd, const char *name, void *data, int size)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj) {
		retVal = yaffs_get_xattrib(obj,name,data,size);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* bad handle */
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

static int yaffs_do_listxattr(const YCHAR *path, char *data, int size, int follow)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(follow)
		obj = yaffsfs_FollowLink(obj,0);

	if(obj) {
		retVal = yaffs_list_xattrib(obj, data,size);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* todo error not found */
		yaffsfs_SetError(-ENOENT);

	yaffsfs_Unlock();

	return retVal;

}

int yaffs_listxattr(const YCHAR *path, char *data, int size)
{
	return yaffs_do_listxattr(path, data, size, 1);
}

int yaffs_llistxattr(const YCHAR *path, char *data, int size)
{
	return yaffs_do_listxattr(path, data, size, 0);
}

int yaffs_flistxattr(int fd, char *data, int size)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj) {
		retVal = yaffs_list_xattrib(obj,data,size);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* bad handle */
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

static int yaffs_do_removexattr(const YCHAR *path, const char *name, int follow)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(follow)
		obj = yaffsfs_FollowLink(obj,0);

	if(obj) {
		retVal = yaffs_remove_xattrib(obj,name);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	} else
		/* todo error not found */
		yaffsfs_SetError(-ENOENT);

	yaffsfs_Unlock();

	return retVal;

}

int yaffs_removexattr(const YCHAR *path, const char *name)
{
	return yaffs_do_removexattr(path, name, 1);
}

int yaffs_lremovexattr(const YCHAR *path, const char *name)
{
	return yaffs_do_removexattr(path, name, 0);
}

int yaffs_fremovexattr(int fd, const char *name)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj){
		retVal = yaffs_remove_xattrib(obj,name);
		if(retVal< 0){
			yaffsfs_SetError(retVal);
			retVal = -1;
		}
	}else
		/* bad handle */
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}
#endif

#ifdef CONFIG_YAFFS_WINCE
int yaffs_get_wince_times(int fd, unsigned *wctime, unsigned *watime, unsigned *wmtime)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj){

		if(wctime){
			wctime[0] = obj->win_ctime[0];
			wctime[1] = obj->win_ctime[1];
		}
		if(watime){
			watime[0] = obj->win_atime[0];
			watime[1] = obj->win_atime[1];
		}
		if(wmtime){
			wmtime[0] = obj->win_mtime[0];
			wmtime[1] = obj->win_mtime[1];
		}


		retVal = 0;
	} else
		/*  bad handle */
		yaffsfs_SetError(-EBADF);		
	
	yaffsfs_Unlock();
	
	return retVal;
}


int yaffs_set_wince_times(int fd, 
						  const unsigned *wctime, 
						  const unsigned *watime, 
                                                  const unsigned *wmtime)
{
        yaffs_obj_t *obj;
        int result;
        int retVal = -1;

        yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj){

		if(wctime){
			obj->win_ctime[0] = wctime[0];
			obj->win_ctime[1] = wctime[1];
		}
		if(watime){
                        obj->win_atime[0] = watime[0];
                        obj->win_atime[1] = watime[1];
                }
                if(wmtime){
                        obj->win_mtime[0] = wmtime[0];
                        obj->win_mtime[1] = wmtime[1];
                }

                obj->dirty = 1;
                result = yaffs_flush_file(obj,0,0);
                retVal = 0;
        } else
		/* bad handle */
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

#endif


static int yaffsfs_DoChMod(yaffs_obj_t *obj,mode_t mode)
{
	int result = -1;

	if(obj)
		obj = yaffs_get_equivalent_obj(obj);

	if(obj) {
		obj->yst_mode = mode;
		obj->dirty = 1;
		result = yaffs_flush_file(obj,0,0);
	}

	return result == YAFFS_OK ? 0 : -1;
}


int yaffs_access(const YCHAR *path, int amode)
{
	yaffs_obj_t *obj;

	int retval = 0;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(obj)	{
		int access_ok = 1;

		if((amode & R_OK) && !(obj->yst_mode & S_IREAD))
			access_ok = 0;
		if((amode & W_OK) && !(obj->yst_mode & S_IWRITE))
			access_ok = 0;
		if((amode & X_OK) && !(obj->yst_mode & S_IEXEC))
			access_ok = 0;

		if(!access_ok) {
			yaffsfs_SetError(-EACCES);
			retval = -1;
		}
	} else {
		/* todo error not found */
		yaffsfs_SetError(-ENOENT);
		retval = -1;
	}

	yaffsfs_Unlock();

	return retval;

}


int yaffs_chmod(const YCHAR *path, mode_t mode)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->my_dev->read_only)
		yaffsfs_SetError(-EINVAL);
	else
		retVal = yaffsfs_DoChMod(obj,mode);

	yaffsfs_Unlock();

	return retVal;

}


int yaffs_fchmod(int fd, mode_t mode)
{
	yaffs_obj_t *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->my_dev->read_only)
		yaffsfs_SetError(-EINVAL);
	else
		retVal = yaffsfs_DoChMod(obj,mode);

	yaffsfs_Unlock();

	return retVal;
}


int yaffs_mkdir(const YCHAR *path, mode_t mode)
{
	yaffs_obj_t *parent = NULL;
	yaffs_obj_t *dir = NULL;
	YCHAR *name;
	int retVal= -1;

	yaffsfs_Lock();
	parent = yaffsfs_FindDirectory(NULL,path,&name,0);
	if(parent && yaffs_strnlen(name,5) == 0){
		/* Trying to make the root itself */
		yaffsfs_SetError(-EEXIST);
	} else if(parent && parent->my_dev->read_only){
		yaffsfs_SetError(-EINVAL);
	} else {
		if(parent)
			dir = yaffs_create_dir(parent,name,mode,0,0);
		if(dir)
			retVal = 0;
		else {
			if(!parent)
				yaffsfs_SetError(-ENOENT); /* missing path */
			else if (yaffs_find_by_name(parent,name))
				yaffsfs_SetError(-EEXIST); /* the name already exists */
			else
				yaffsfs_SetError(-ENOSPC); /* just assume no space */
			retVal = -1;
		}
	}

	yaffsfs_Unlock();

	return retVal;
}

void * yaffs_getdev(const YCHAR *path)
{
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;
	dev = yaffsfs_FindDevice(path,&dummy);
	return (void *)dev;
}

int yaffs_mount2(const YCHAR *path,int read_only)
{
	int retVal=-1;
	int result=YAFFS_FAIL;
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;

	T(YAFFS_TRACE_ALWAYS,(TSTR("yaffs: Mounting %s" TENDSTR),path));

	yaffsfs_Lock();

	yaffsfs_InitHandles();

	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev){
		if(!dev->is_mounted){
			dev->read_only = read_only ? 1 : 0;
			result = yaffs_guts_initialise(dev);
			if(result == YAFFS_FAIL)
				/* todo error - mount failed */
				yaffsfs_SetError(-ENOMEM);
			retVal = result ? 0 : -1;

		}
		else
			/* todo error - already mounted. */
			yaffsfs_SetError(-EBUSY);
	} else
		/* todo error - no device */
		yaffsfs_SetError(-ENODEV);

	yaffsfs_Unlock();
	return retVal;

}

int yaffs_mount(const YCHAR *path)
{
	return yaffs_mount2(path,0);
}

int yaffs_sync(const YCHAR *path)
{
        int retVal=-1;
        yaffs_dev_t *dev=NULL;
        YCHAR *dummy;
        
        yaffsfs_Lock();
        dev = yaffsfs_FindDevice(path,&dummy);
        if(dev){
                if(dev->is_mounted){
                        
                        yaffs_flush_whole_cache(dev);
                        yaffs_checkpoint_save(dev);
                        retVal = 0;
                        
                } else
                        /* todo error - not mounted. */
                        yaffsfs_SetError(-EINVAL);
                        
        }else
                /* todo error - no device */
                yaffsfs_SetError(-ENODEV);

        yaffsfs_Unlock();
        return retVal;  
}


int yaffs_remount(const YCHAR *path, int force, int read_only)
{
        int retVal=-1;
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev){
		if(dev->is_mounted){
			int i;
			int inUse;

			yaffs_flush_whole_cache(dev);

			for(i = inUse = 0; i < YAFFSFS_N_HANDLES && !inUse && !force; i++){
				if(yaffsfs_handle[i].useCount>0 && yaffsfs_inode[yaffsfs_handle[i].inodeId].iObj->my_dev == dev)
					inUse = 1; /* the device is in use, can't unmount */
			}

			if(!inUse || force){
				if(read_only)
					yaffs_checkpoint_save(dev);
				dev->read_only =  read_only ? 1 : 0;
				retVal = 0;
			} else
				yaffsfs_SetError(-EBUSY);

		} else
			yaffsfs_SetError(-EINVAL);

	}
	else
		yaffsfs_SetError(-ENODEV);

	yaffsfs_Unlock();
	return retVal;

}

int yaffs_unmount2(const YCHAR *path, int force)
{
        int retVal=-1;
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev){
		if(dev->is_mounted){
			int i;
			int inUse;

			yaffs_flush_whole_cache(dev);
			yaffs_checkpoint_save(dev);

			for(i = inUse = 0; i < YAFFSFS_N_HANDLES && !inUse; i++){
				if(yaffsfs_handle[i].useCount > 0 && yaffsfs_inode[yaffsfs_handle[i].inodeId].iObj->my_dev == dev)
					inUse = 1; /* the device is in use, can't unmount */
			}

			if(!inUse || force){
				yaffs_deinitialise(dev);

				retVal = 0;
			} else
				/* todo error can't unmount as files are open */
				yaffsfs_SetError(-EBUSY);

		} else
			/* todo error - not mounted. */
			yaffsfs_SetError(-EINVAL);

	}
	else
		/* todo error - no device */
		yaffsfs_SetError(-ENODEV);

	yaffsfs_Unlock();
	return retVal;

}

int yaffs_unmount(const YCHAR *path)
{
	return yaffs_unmount2(path,0);
}

loff_t yaffs_freespace(const YCHAR *path)
{
	loff_t retVal=-1;
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev  && dev->is_mounted){
		retVal = yaffs_get_n_free_chunks(dev);
		retVal *= dev->data_bytes_per_chunk;

	} else
		yaffsfs_SetError(-EINVAL);

	yaffsfs_Unlock();
	return retVal;
}

loff_t yaffs_totalspace(const YCHAR *path)
{
	loff_t retVal=-1;
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev  && dev->is_mounted){
		retVal = (dev->param.end_block - dev->param.start_block + 1) - dev->param.n_reserved_blocks;
		retVal *= dev->param.chunks_per_block;
		retVal *= dev->data_bytes_per_chunk;

	} else
		yaffsfs_SetError(-EINVAL);

	yaffsfs_Unlock();
	return retVal;
}

int yaffs_inodecount(const YCHAR *path)
{
	loff_t retVal= -1;
	yaffs_dev_t *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev  && dev->is_mounted) {
	   int n_obj = dev->n_obj;
	   if(n_obj > dev->n_hardlinks)
		retVal = n_obj - dev->n_hardlinks;
	}
	
	if(retVal < 0)
		yaffsfs_SetError(-EINVAL);
	
	yaffsfs_Unlock();
	return retVal;	
}


void yaffs_add_device(yaffs_dev_t *dev)
{
	dev->is_mounted = 0;
	dev->param.remove_obj_fn = yaffsfs_RemoveObjectCallback;

	if(!dev->dev_list.next)
		YINIT_LIST_HEAD(&dev->dev_list);

	ylist_add(&dev->dev_list,&yaffsfs_deviceList);
}

void yaffs_remove_device(yaffs_dev_t *dev)
{
	ylist_del_init(&dev->dev_list);
}




/* Directory search stuff. */

/*
 * Directory search context
 *
 * NB this is an opaque structure.
 */


typedef struct
{
	__u32 magic;
	yaffs_dirent de;		/* directory entry being used by this dsc */
	YCHAR name[NAME_MAX+1];		/* name of directory being searched */
        yaffs_obj_t *dirObj;           /* ptr to directory being searched */
        yaffs_obj_t *nextReturn;       /* obj to be returned by next readddir */
        int offset;
        struct ylist_head others;       
} yaffsfs_DirectorySearchContext;



static struct ylist_head search_contexts;


static void yaffsfs_SetDirRewound(yaffsfs_DirectorySearchContext *dsc)
{
	if(dsc &&
	   dsc->dirObj &&
	   dsc->dirObj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY){

           dsc->offset = 0;

           if( ylist_empty(&dsc->dirObj->variant.dir_variant.children))
                dsc->nextReturn = NULL;
           else
                dsc->nextReturn = ylist_entry(dsc->dirObj->variant.dir_variant.children.next,
                                                yaffs_obj_t,siblings);
        } else {
		/* Hey someone isn't playing nice! */
	}
}

static void yaffsfs_DirAdvance(yaffsfs_DirectorySearchContext *dsc)
{
	if(dsc &&
	   dsc->dirObj &&
           dsc->dirObj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY){

           if( dsc->nextReturn == NULL ||
               ylist_empty(&dsc->dirObj->variant.dir_variant.children))
                dsc->nextReturn = NULL;
           else {
                   struct ylist_head *next = dsc->nextReturn->siblings.next;

                   if( next == &dsc->dirObj->variant.dir_variant.children)
                        dsc->nextReturn = NULL; /* end of list */
                   else
                        dsc->nextReturn = ylist_entry(next,yaffs_obj_t,siblings);
           }
        } else {
                /* Hey someone isn't playing nice! */
	}
}

static void yaffsfs_RemoveObjectCallback(yaffs_obj_t *obj)
{

        struct ylist_head *i;
        yaffsfs_DirectorySearchContext *dsc;

        /* if search contexts not initilised then skip */
        if(!search_contexts.next)
                return;

        /* Iterate through the directory search contexts.
         * If any are the one being removed, then advance the dsc to
         * the next one to prevent a hanging ptr.
         */
         ylist_for_each(i, &search_contexts) {
                if (i) {
                        dsc = ylist_entry(i, yaffsfs_DirectorySearchContext,others);
                        if(dsc->nextReturn == obj)
                                yaffsfs_DirAdvance(dsc);
                }
	}

}

yaffs_DIR *yaffs_opendir(const YCHAR *dirname)
{
	yaffs_DIR *dir = NULL;
 	yaffs_obj_t *obj = NULL;
	yaffsfs_DirectorySearchContext *dsc = NULL;

	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,dirname,0);

	if(obj && obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY){

		dsc = YMALLOC(sizeof(yaffsfs_DirectorySearchContext));
		dir = (yaffs_DIR *)dsc;

		if(dsc){
			memset(dsc,0,sizeof(yaffsfs_DirectorySearchContext));
                        dsc->magic = YAFFS_MAGIC;
                        dsc->dirObj = obj;
                        yaffs_strncpy(dsc->name,dirname,NAME_MAX);
                        YINIT_LIST_HEAD(&dsc->others);

                        if(!search_contexts.next)
                                YINIT_LIST_HEAD(&search_contexts);

                        ylist_add(&dsc->others,&search_contexts);       
                        yaffsfs_SetDirRewound(dsc);
		}

        }

	yaffsfs_Unlock();

	return dir;
}

struct yaffs_dirent *yaffs_readdir(yaffs_DIR *dirp)
{
	yaffsfs_DirectorySearchContext *dsc = (yaffsfs_DirectorySearchContext *)dirp;
	struct yaffs_dirent *retVal = NULL;

	yaffsfs_Lock();

	if(dsc && dsc->magic == YAFFS_MAGIC){
		yaffsfs_SetError(0);
		if(dsc->nextReturn){
			dsc->de.d_ino = yaffs_get_equivalent_obj(dsc->nextReturn)->obj_id;
			dsc->de.d_dont_use = (unsigned)dsc->nextReturn;
			dsc->de.d_off = dsc->offset++;
			yaffs_get_obj_name(dsc->nextReturn,dsc->de.d_name,NAME_MAX);
			if(yaffs_strnlen(dsc->de.d_name,NAME_MAX+1) == 0)
			{
				/* this should not happen! */
				yaffs_strcpy(dsc->de.d_name,_Y("zz"));
			}
			dsc->de.d_reclen = sizeof(struct yaffs_dirent);
			retVal = &dsc->de;
			yaffsfs_DirAdvance(dsc);
		} else
			retVal = NULL;
	} else
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;

}


void yaffs_rewinddir(yaffs_DIR *dirp)
{
	yaffsfs_DirectorySearchContext *dsc = (yaffsfs_DirectorySearchContext *)dirp;

	yaffsfs_Lock();

	yaffsfs_SetDirRewound(dsc);

	yaffsfs_Unlock();
}


int yaffs_closedir(yaffs_DIR *dirp)
{
	yaffsfs_DirectorySearchContext *dsc = (yaffsfs_DirectorySearchContext *)dirp;

        yaffsfs_Lock();
        dsc->magic = 0;
        ylist_del(&dsc->others); /* unhook from list */
        YFREE(dsc);
        yaffsfs_Unlock();
        return 0;
}

/* End of directory stuff */


int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath)
{
	yaffs_obj_t *parent = NULL;
	yaffs_obj_t *obj;
	YCHAR *name;
	int retVal= -1;
	int mode = 0; /* ignore for now */

	yaffsfs_Lock();
	parent = yaffsfs_FindDirectory(NULL,newpath,&name,0);
	if(parent && parent->my_dev->read_only)
		yaffsfs_SetError(-EINVAL);
	else if(parent){
		obj = yaffs_create_symlink(parent,name,mode,0,0,oldpath);
		if(obj)
			retVal = 0;
		else{
			yaffsfs_SetError(-ENOSPC); /* just assume no space for now */
			retVal = -1;
		}
	} else {
		yaffsfs_SetError(-EINVAL);
		retVal = -1;
	}

	yaffsfs_Unlock();

	return retVal;

}

int yaffs_readlink(const YCHAR *path, YCHAR *buf, int bufsiz)
{
	yaffs_obj_t *obj = NULL;
	int retVal;


	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,path,0);

	if(!obj) {
		yaffsfs_SetError(-ENOENT);
		retVal = -1;
	} else if(obj->variant_type != YAFFS_OBJECT_TYPE_SYMLINK) {
		yaffsfs_SetError(-EINVAL);
		retVal = -1;
	} else {
		YCHAR *alias = obj->variant.symlink_variant.alias;
		memset(buf,0,bufsiz);
		yaffs_strncpy(buf,alias,bufsiz - 1);
		retVal = 0;
	}
	yaffsfs_Unlock();
	return retVal;
}

int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath)
{
	/* Creates a link called newpath to existing oldpath */
	yaffs_obj_t *obj = NULL;
	yaffs_obj_t *target = NULL;
	int retVal = 0;
	int new_nameLength = 0;


	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,oldpath,0);
	target = yaffsfs_FindObject(NULL,newpath,0);

	if(!obj) {
		yaffsfs_SetError(-ENOENT);
		retVal = -1;
	} else if(obj->my_dev->read_only){
		yaffsfs_SetError(-EINVAL);
		retVal= -1;
	} else if(target) {
		yaffsfs_SetError(-EEXIST);
		retVal = -1;
	} else {
		yaffs_obj_t *newdir = NULL;
		yaffs_obj_t *link = NULL;

		YCHAR *newname;

		newdir = yaffsfs_FindDirectory(NULL,newpath,&newname,0);

		if(!newdir){
			yaffsfs_SetError(-ENOTDIR);
			retVal = -1;
		}else if(newdir->my_dev != obj->my_dev){
			yaffsfs_SetError(-EXDEV);
			retVal = -1;
		}
		
		new_nameLength = yaffs_strnlen(newname,YAFFS_MAX_NAME_LENGTH+1);
		
		if(new_nameLength == 0){
			yaffsfs_SetError(-ENOENT);
			retVal = -1;
		} else if (new_nameLength > YAFFS_MAX_NAME_LENGTH){
			yaffsfs_SetError(-ENAMETOOLONG);
			retVal = -1;
		}
		
		if(retVal == 0) {
			link = yaffs_link_obj(newdir,newname,obj);
			if(link)
				retVal = 0;
			else{
				yaffsfs_SetError(-ENOSPC);
				retVal = -1;
			}

		}
	}
	yaffsfs_Unlock();

	return retVal;
}

int yaffs_mknod(const YCHAR *pathname, mode_t mode, dev_t dev)
{
	return -1;
}



/*
 * yaffs_n_handles()
 * Returns number of handles attached to the object
 */
int yaffs_n_handles(const YCHAR *path)
{
	yaffs_obj_t *obj;

	obj = yaffsfs_FindObject(NULL,path,0);
	if(obj)
		obj = yaffs_get_equivalent_obj(obj);

	return yaffsfs_CountHandles(obj);
}

int yaffs_dump_dev(const YCHAR *path)
{
#if 0
	YCHAR *rest;

	yaffs_obj_t *obj = yaffsfs_FindRoot(path,&rest);

	if(obj){
		yaffs_dev_t *dev = obj->my_dev;

		printf("\n"
			   "n_page_writes.......... %d\n"
			   "n_page_reads........... %d\n"
			   "n_erasures....... %d\n"
			   "n_gc_copies............ %d\n"
			   "garbageCollections... %d\n"
			   "passiveGarbageColl'ns %d\n"
			   "\n",
				dev->n_page_writes,
				dev->n_page_reads,
				dev->n_erasures,
				dev->n_gc_copies,
				dev->garbageCollections,
				dev->passiveGarbageCollections
		);

	}

#endif
	return 0;
}

