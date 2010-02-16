/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
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

#include <string.h> // for memset

#define YAFFSFS_MAX_SYMLINK_DEREFERENCES 5

#ifndef NULL
#define NULL ((void *)0)
#endif


/* YAFFSFS_RW_SIZE must be a power of 2 */
#define YAFFSFS_RW_SHIFT (13)
#define YAFFSFS_RW_SIZE  (1<<YAFFSFS_RW_SHIFT)


const char *yaffsfs_c_version="$Id: yaffsfs.c,v 1.33 2010-02-16 23:24:57 charles Exp $";

// configurationList is the list of devices that are supported
static yaffsfs_DeviceConfiguration *yaffsfs_configurationList;


/* Some forward references */
static yaffs_Object *yaffsfs_FindObject(yaffs_Object *relativeDirectory, const YCHAR *path, int symDepth);
static void yaffsfs_RemoveObjectCallback(yaffs_Object *obj);


// Handle management.
//


unsigned int yaffs_wr_attempts;

typedef struct {
	int count;
	yaffs_Object *iObj;
} yaffsfs_Inode;

typedef struct{
	__u8 	readOnly:1;	// this handle is read only
	__u8 	append:1;	// append only
	__u8	exclusive:1;	// exclusive
	int	inodeId:13;	// the object
	int	useCount:16;	// Use count for this handle
	__u32 position;		// current position in file
}yaffsfs_Handle;

static yaffsfs_Inode yaffsfs_inode[YAFFSFS_N_HANDLES];
static yaffsfs_Handle yaffsfs_handle[YAFFSFS_N_HANDLES];

// yaffsfs_InitHandle
/// Inilitalise handles on start-up.
//
static int yaffsfs_InitHandles(void)
{
	int i;
	memset(yaffsfs_inode,0,sizeof(yaffsfs_inode));
	memset(yaffsfs_handle,0,sizeof(yaffsfs_handle));
	for(i = 0; i < YAFFSFS_N_HANDLES; i++)
		yaffsfs_handle[i].inodeId = -1;

	return 0;
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

yaffs_Object *yaffsfs_GetHandleObject(int handle)
{
	yaffsfs_Inode *in = yaffsfs_GetInodePointer(handle);

	if(in)
		return in->iObj;

	return NULL;
}

//yaffsfs_GetInodeIdForObject
// Grab an inode entry when opening a new inode.
//

static int yaffsfs_GetInodeIdForObject(yaffs_Object *obj)
{
	int i;
	int ret = -1;
	yaffsfs_Inode *in = NULL;
	
	if(obj)
		obj = yaffs_GetEquivalentObject(obj);

	/* Look for it. If we can't find it then make one */
	for(i = 0; i < YAFFSFS_N_HANDLES && ret < 0; i++){
		if(yaffsfs_inode[i].iObj == obj)
			ret = i;
	}

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

static void yaffsfs_ReleaseInode(yaffsfs_Inode *in)
{
	yaffs_Object *obj;
	
	obj = in->iObj;

	if(obj->unlinked)
		yaffs_DeleteObject(obj);
	
	obj->myInode = NULL;
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


//yaffsfs_GetHandle
// Grab a handle (when opening a file)
//

static int yaffsfs_GetNewHandle(void)
{
	int i;
	yaffsfs_Handle *h;

	for(i = 0; i < YAFFSFS_N_HANDLES; i++){
		h = yaffsfs_GetHandlePointer(i);
		if(!h){
			// todo bug: should never happen
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

// yaffs_GetHandle
// Increase use of handle when reading/writing a file
static int yaffsfs_GetHandle(int handle)
{
	yaffsfs_Handle *h = yaffsfs_GetHandlePointer(handle);

	if(h && h->useCount > 0){	
		h->useCount++;
	}
	return 0;
}

// yaffs_PutHandle
// Let go of a handle when closing a file or aborting an open or
// ending a read or write.
//
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




// Stuff to search for a directory from a path


int yaffsfs_Match(YCHAR a, YCHAR b)
{
	// case sensitive
	return (a == b);
}

int yaffsfs_IsPathDivider(YCHAR ch)
{
	YCHAR *str = YAFFS_PATH_DIVIDERS;

	while(*str){
		if(*str == ch)
			return 1;
		str++;
	}

	return 0;
}

// yaffsfs_FindDevice
// yaffsfs_FindRoot
// Scan the configuration list to find the root.
// Curveballs: Should match paths that end in '/' too
// Curveball2 Might have "/x/ and "/x/y". Need to return the longest match
static yaffs_Device *yaffsfs_FindDevice(const YCHAR *path, YCHAR **restOfPath)
{
	yaffsfs_DeviceConfiguration *cfg = yaffsfs_configurationList;
	const YCHAR *leftOver;
	const YCHAR *p;
	yaffs_Device *retval = NULL;
	int thisMatchLength;
	int longestMatch = -1;
	int matching;

	// Check all configs, choose the one that:
	// 1) Actually matches a prefix (ie /a amd /abc will not match
	// 2) Matches the longest.
	while(cfg && cfg->prefix && cfg->dev){
		leftOver = path;
		p = cfg->prefix;
		thisMatchLength = 0;
		matching = 1;


		while(matching && *p && *leftOver){
			// Skip over any /s
			while(yaffsfs_IsPathDivider(*p))
			      p++;

			// Skip over any /s
			while(yaffsfs_IsPathDivider(*leftOver))
		              leftOver++;

			// Now match the text part
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

		// Skip over any /s in leftOver
		while(yaffsfs_IsPathDivider(*leftOver))
	              leftOver++;
		

		if( matching && (thisMatchLength > longestMatch)){
			// Matched prefix
			*restOfPath = (YCHAR *)leftOver;
			retval = cfg->dev;
			longestMatch = thisMatchLength;
		}

		cfg++;
	}
	return retval;
}

#if 0
static yaffs_Device *yaffsfs_FindDevice(const YCHAR *path, YCHAR **restOfPath)
{
	yaffsfs_DeviceConfiguration *cfg = yaffsfs_configurationList;
	const YCHAR *leftOver;
	const YCHAR *p;
	yaffs_Device *retval = NULL;
	int thisMatchLength;
	int longestMatch = -1;

	// Check all configs, choose the one that:
	// 1) Actually matches a prefix (ie /a amd /abc will not match
	// 2) Matches the longest.
	while(cfg && cfg->prefix && cfg->dev){
		leftOver = path;
		p = cfg->prefix;
		thisMatchLength = 0;

		while(*p &&  //unmatched part of prefix
		      !(yaffsfs_IsPathDivider(*p) && (p[1] == 0)) && // the rest of the prefix is not / (to catch / at end)
		      *leftOver &&
		      yaffsfs_Match(*p,*leftOver)){
			p++;
			leftOver++;
			thisMatchLength++;
		}


		if((!*p || (yaffsfs_IsPathDivider(*p) && (p[1] == 0))) &&      // end of prefix
		   (!*leftOver || yaffsfs_IsPathDivider(*leftOver)) && // no more in this path name part
		   (thisMatchLength > longestMatch)){
			// Matched prefix
			*restOfPath = (YCHAR *)leftOver;
			retval = cfg->dev;
			longestMatch = thisMatchLength;
		}
		cfg++;
	}
	return retval;
}
#endif

static yaffs_Object *yaffsfs_FindRoot(const YCHAR *path, YCHAR **restOfPath)
{

	yaffs_Device *dev;

	dev= yaffsfs_FindDevice(path,restOfPath);
	if(dev && dev->isMounted){
		return dev->rootDir;
	}
	return NULL;
}

static yaffs_Object *yaffsfs_FollowLink(yaffs_Object *obj,int symDepth)
{

	while(obj && obj->variantType == YAFFS_OBJECT_TYPE_SYMLINK){
		YCHAR *alias = obj->variant.symLinkVariant.alias;

		if(yaffsfs_IsPathDivider(*alias))
			// Starts with a /, need to scan from root up
			obj = yaffsfs_FindObject(NULL,alias,symDepth++);
		else
			// Relative to here, so use the parent of the symlink as a start
			obj = yaffsfs_FindObject(obj->parent,alias,symDepth++);
	}
	return obj;
}


// yaffsfs_FindDirectory
// Parse a path to determine the directory and the name within the directory.
//
// eg. "/data/xx/ff" --> puts name="ff" and returns the directory "/data/xx"
static yaffs_Object *yaffsfs_DoFindDirectory(yaffs_Object *startDir,const YCHAR *path,YCHAR **name,int symDepth)
{
	yaffs_Object *dir;
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
		// parse off /.
		// curve ball: also throw away surplus '/'
		// eg. "/ram/x////ff" gets treated the same as "/ram/x/ff"
		while(yaffsfs_IsPathDivider(*restOfPath))
			restOfPath++; // get rid of '/'

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
			// got to the end of the string
			return dir;
		else{
			if(yaffs_strcmp(str,_Y(".")) == 0)
			{
				// Do nothing
			}
			else if(yaffs_strcmp(str,_Y("..")) == 0)
				dir = dir->parent;
			else{
				dir = yaffs_FindObjectByName(dir,str);

				while(dir && dir->variantType == YAFFS_OBJECT_TYPE_SYMLINK)
					dir = yaffsfs_FollowLink(dir,symDepth);


				if(dir && dir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
					dir = NULL;
			}
		}
	}
	// directory did not exist.
	return NULL;
}

static yaffs_Object *yaffsfs_FindDirectory(yaffs_Object *relativeDirectory,const YCHAR *path,YCHAR **name,int symDepth)
{
	return yaffsfs_DoFindDirectory(relativeDirectory,path,name,symDepth);
}

// yaffsfs_FindObject turns a path for an existing object into the object
//
static yaffs_Object *yaffsfs_FindObject(yaffs_Object *relativeDirectory, const YCHAR *path,int symDepth)
{
	yaffs_Object *dir;
	YCHAR *name;

	dir = yaffsfs_FindDirectory(relativeDirectory,path,&name,symDepth);

	if(dir && *name)
		return yaffs_FindObjectByName(dir,name);

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

int yaffs_open(const YCHAR *path, int oflag, int mode)
{
	yaffs_Object *obj = NULL;
	yaffs_Object *dir = NULL;
	YCHAR *name;
	int handle = -1;
	yaffsfs_Handle *h = NULL;
	int alreadyOpen = 0;
	int alreadyExclusive = 0;
	int openDenied = 0;
	int symDepth = 0;
	int errorReported = 0;

	int i;


	// todo sanity check oflag (eg. can't have O_TRUNC without WRONLY or RDWR


	yaffsfs_Lock();

	handle = yaffsfs_GetNewHandle();

	if(handle >= 0){

		h = yaffsfs_GetHandlePointer(handle);


		// try to find the exisiting object
		obj = yaffsfs_FindObject(NULL,path,0);

		if(obj && obj->variantType == YAFFS_OBJECT_TYPE_SYMLINK)
			obj = yaffsfs_FollowLink(obj,symDepth++);

		if(obj)
			obj = yaffs_GetEquivalentObject(obj);

		if(obj && obj->variantType != YAFFS_OBJECT_TYPE_FILE)
			obj = NULL;

		if(obj){
			// Check if the object is already in use
			alreadyOpen = alreadyExclusive = 0;

			for(i = 0; i < YAFFSFS_N_HANDLES; i++){
				if(i != handle &&
				   yaffsfs_handle[i].useCount > 0 &&
				    obj == yaffsfs_inode[yaffsfs_handle[i].inodeId].iObj){
				 	alreadyOpen = 1;
					if(yaffsfs_handle[i].exclusive)
						alreadyExclusive = 1;
				 }
			}

			if(((oflag & O_EXCL) && alreadyOpen) || alreadyExclusive)
				openDenied = 1;

			// Open should fail if O_CREAT and O_EXCL are specified
			if((oflag & O_EXCL) && (oflag & O_CREAT)){
				openDenied = 1;
				yaffsfs_SetError(-EEXIST);
				errorReported = 1;
			}

			// Check file permissions
			if( (oflag & (O_RDWR | O_WRONLY)) == 0 &&     // ie O_RDONLY
			   !(obj->yst_mode & S_IREAD))
				openDenied = 1;

			if( (oflag & O_RDWR) &&
			   !(obj->yst_mode & S_IREAD))
				openDenied = 1;

			if( (oflag & (O_RDWR | O_WRONLY)) &&
			   !(obj->yst_mode & S_IWRITE))
				openDenied = 1;

		} else if((oflag & O_CREAT)) {
			// Let's see if we can create this file
			dir = yaffsfs_FindDirectory(NULL,path,&name,0);
			if(dir  && dir->myDev->readOnly){
				yaffsfs_SetError(-EINVAL);
				errorReported = 1;
			} else if(dir)
				obj = yaffs_MknodFile(dir,name,mode,0,0);
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
			h->readOnly = (oflag & (O_WRONLY | O_RDWR)) ? 0 : 1;
			h->append =  (oflag & O_APPEND) ? 1 : 0;
			h->exclusive = (oflag & O_EXCL) ? 1 : 0;
			h->position = 0;

			/* Hook inode to object */
                        obj->myInode = (void*) &yaffsfs_inode[inodeId];

                        if((oflag & O_TRUNC) && !h->readOnly)
                                yaffs_ResizeFile(obj,0);
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

int yaffs_Dofsync(int fd,int datasync)
{
	yaffsfs_Handle *h = NULL;
	int retVal = 0;

	yaffsfs_Lock();

	h = yaffsfs_GetHandlePointer(fd);

	if(h && h->useCount > 0)
		// flush the file
		yaffs_FlushFile(yaffsfs_inode[h->inodeId].iObj,1,datasync);
	else {
		// bad handle
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
		// clean up
		yaffs_FlushFile(yaffsfs_inode[h->inodeId].iObj,1,0);
		yaffsfs_PutHandle(fd);
		retVal = 0;
	} else {
		// bad handle
		yaffsfs_SetError(-EBADF);
		retVal = -1;
	}

	yaffsfs_Unlock();

	return retVal;
}



int yaffsfs_do_read(int fd, void *buf, unsigned int nbyte, int isPread, int offset)
{
	yaffsfs_Handle *h = NULL;
	yaffs_Object *obj = NULL;
	int pos = 0;
	int startPos = 0;
	int nRead = 0;
	int nToRead = 0;
	int totalRead = 0;
	unsigned int maxRead;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj){
		// bad handle
		yaffsfs_SetError(-EBADF);
		totalRead = -1;
	} else if( h && obj){
		if(isPread)
			startPos = offset;
		else
			startPos = h->position;
			
		if(yaffs_GetObjectFileLength(obj) > pos)
			maxRead = yaffs_GetObjectFileLength(obj) - pos;
		else
			maxRead = 0;

		if(nbyte > maxRead)
			nbyte = maxRead;


		if(nbyte > 0) {
			yaffsfs_GetHandle(fd);
			pos = startPos;
			
			while(nbyte > 0) {
				nToRead = YAFFSFS_RW_SIZE - (pos & (YAFFSFS_RW_SIZE -1));
				if(nToRead > nbyte)
					nToRead = nbyte;

				nRead = yaffs_ReadDataFromFile(obj,buf,pos,nToRead);

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
					//todo error
				}
			}
		} else
			totalRead = 0;

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

int yaffsfs_do_write(int fd, const void *buf, unsigned int nbyte, int isPwrite, int offset)
{
	yaffsfs_Handle *h = NULL;
	yaffs_Object *obj = NULL;
	int pos = 0;
	int startPos = 0;
	int nWritten = 0;
	int totalWritten = 0;
	int writeThrough = 0;
	int nToWrite = 0;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj){
		// bad handle
		yaffsfs_SetError(-EBADF);
		totalWritten = -1;
	} else if( h && obj && (h->readOnly || obj->myDev->readOnly)){
		yaffsfs_SetError(-EINVAL);
		totalWritten=-1;
	} else if( h && obj){
		if(isPwrite)
			startPos = offset;
		if(h->append)
			startPos = yaffs_GetObjectFileLength(obj);
		else
			startPos = h->position;
		if( nbyte > 0){
			yaffsfs_GetHandle(fd);
			pos = startPos;
			while(nbyte > 0) {
				nToWrite = YAFFSFS_RW_SIZE - (pos & (YAFFSFS_RW_SIZE -1));
				if(nToWrite > nbyte)
					nToWrite = nbyte;
				
				nWritten = yaffs_WriteDataToFile(obj,buf,pos,nToWrite,writeThrough);
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
					//todo error
				}
			}
		} else
			totalWritten = 0;

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


int yaffs_truncate(const YCHAR *path,off_t newSize)
{
	yaffs_Object *obj = NULL;
	int result = YAFFS_FAIL;

	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,path,0);
	if(obj)
		obj = yaffs_GetEquivalentObject(obj);

	if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->variantType != YAFFS_OBJECT_TYPE_FILE)
		yaffsfs_SetError(-EISDIR);
	else if(obj->myDev->readOnly)
		yaffsfs_SetError(-EINVAL);
	else
		result = yaffs_ResizeFile(obj,newSize);

	yaffsfs_Unlock();


	return (result) ? 0 : -1;
}

int yaffs_ftruncate(int fd, off_t newSize)
{
	yaffsfs_Handle *h = NULL;
	yaffs_Object *obj = NULL;
	int result = 0;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj)
		// bad handle
		yaffsfs_SetError(-EBADF);
	else if(obj->myDev->readOnly)
		yaffsfs_SetError(-EINVAL);
	else
		// resize the file
		result = yaffs_ResizeFile(obj,newSize);
	yaffsfs_Unlock();


	return (result) ? 0 : -1;

}

off_t yaffs_lseek(int fd, off_t offset, int whence)
{
	yaffsfs_Handle *h = NULL;
	yaffs_Object *obj = NULL;
	int pos = -1;
	int fSize = -1;

	yaffsfs_Lock();
	h = yaffsfs_GetHandlePointer(fd);
	obj = yaffsfs_GetHandleObject(fd);

	if(!h || !obj)
		// bad handle
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
		fSize = yaffs_GetObjectFileLength(obj);
		if(fSize >= 0 && (fSize + offset) >= 0)
			pos = fSize + offset;
	}

	if(pos >= 0)
		h->position = pos;
	else {
		// todo error
	}


	yaffsfs_Unlock();

	return pos;
}


int yaffsfs_DoUnlink(const YCHAR *path,int isDirectory)
{
	yaffs_Object *dir = NULL;
	yaffs_Object *obj = NULL;
	YCHAR *name;
	int result = YAFFS_FAIL;

	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,path,0);
	dir = yaffsfs_FindDirectory(NULL,path,&name,0);
	if(!dir)
		yaffsfs_SetError(-ENOTDIR);
	else if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->myDev->readOnly)
		yaffsfs_SetError(-EINVAL);
	else if(!isDirectory && obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY)
		yaffsfs_SetError(-EISDIR);
	else if(isDirectory && obj->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
		yaffsfs_SetError(-ENOTDIR);
	else {
		result = yaffs_Unlink(dir,name);

		if(result == YAFFS_FAIL && isDirectory)
			yaffsfs_SetError(-ENOTEMPTY);
	}

	yaffsfs_Unlock();

	// todo error

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
	yaffs_Object *olddir = NULL;
	yaffs_Object *newdir = NULL;
	yaffs_Object *obj = NULL;
	YCHAR *oldname;
	YCHAR *newname;
	int result= YAFFS_FAIL;
	int renameAllowed = 1;

	yaffsfs_Lock();

	olddir = yaffsfs_FindDirectory(NULL,oldPath,&oldname,0);
	newdir = yaffsfs_FindDirectory(NULL,newPath,&newname,0);
	obj = yaffsfs_FindObject(NULL,oldPath,0);

	if(!olddir || !newdir || !obj) {
		// bad file
		yaffsfs_SetError(-EBADF);
		renameAllowed = 0;
	} else if(obj->myDev->readOnly){
		yaffsfs_SetError(-EINVAL);
		renameAllowed = 0;
	} else if(olddir->myDev != newdir->myDev) {
		// oops must be on same device
		// todo error
		yaffsfs_SetError(-EXDEV);
		renameAllowed = 0;
	} else if(obj && obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY) {
		// It is a directory, check that it is not being renamed to
		// being its own decendent.
		// Do this by tracing from the new directory back to the root, checking for obj

		yaffs_Object *xx = newdir;

		while( renameAllowed && xx){
			if(xx == obj)
				renameAllowed = 0;
			xx = xx->parent;
		}
		if(!renameAllowed)
			yaffsfs_SetError(-EACCES);
	}

	if(renameAllowed)
		result = yaffs_RenameObject(olddir,oldname,newdir,newname);

	yaffsfs_Unlock();

	return (result == YAFFS_FAIL) ? -1 : 0;
}


static int yaffsfs_DoStat(yaffs_Object *obj,struct yaffs_stat *buf)
{
	int retVal = -1;

	if(obj)
		obj = yaffs_GetEquivalentObject(obj);

	if(obj && buf){
	    	buf->st_dev = (int)obj->myDev->genericDevice;
	    	buf->st_ino = obj->objectId;
	    	buf->st_mode = obj->yst_mode & ~S_IFMT; // clear out file type bits

	    	if(obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY)
			buf->st_mode |= S_IFDIR;
		else if(obj->variantType == YAFFS_OBJECT_TYPE_SYMLINK)
			buf->st_mode |= S_IFLNK;
		else if(obj->variantType == YAFFS_OBJECT_TYPE_FILE)
			buf->st_mode |= S_IFREG;

	    	buf->st_nlink = yaffs_GetObjectLinkCount(obj);
	    	buf->st_uid = 0;
	    	buf->st_gid = 0;;
	    	buf->st_rdev = obj->yst_rdev;
	    	buf->st_size = yaffs_GetObjectFileLength(obj);
	    	buf->st_blksize = obj->myDev->nDataBytesPerChunk;
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
	yaffs_Object *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(!doLStat && obj)
		obj = yaffsfs_FollowLink(obj,0);

	if(obj)
		retVal = yaffsfs_DoStat(obj,buf);
	else
		// todo error not found
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
	yaffs_Object *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(obj)
		retVal = yaffsfs_DoStat(obj,buf);
	else
		// bad handle
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

#ifdef CONFIG_YAFFS_WINCE
int yaffs_get_wince_times(int fd, unsigned *wctime, unsigned *watime, unsigned *wmtime)
{
	yaffs_Object *obj;

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
		// bad handle
		yaffsfs_SetError(-EBADF);		
	
	yaffsfs_Unlock();
	
	return retVal;
}


int yaffs_set_wince_times(int fd, 
						  const unsigned *wctime, 
						  const unsigned *watime, 
                                                  const unsigned *wmtime)
{
        yaffs_Object *obj;
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
                result = yaffs_FlushFile(obj,0,0);
                retVal = 0;
        } else
		// bad handle
		yaffsfs_SetError(-EBADF);

	yaffsfs_Unlock();

	return retVal;
}

#endif


static int yaffsfs_DoChMod(yaffs_Object *obj,mode_t mode)
{
	int result = -1;

	if(obj)
		obj = yaffs_GetEquivalentObject(obj);

	if(obj) {
		obj->yst_mode = mode;
		obj->dirty = 1;
		result = yaffs_FlushFile(obj,0,0);
	}

	return result == YAFFS_OK ? 0 : -1;
}


int yaffs_access(const YCHAR *path, int amode)
{
	yaffs_Object *obj;

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
		// todo error not found
		yaffsfs_SetError(-ENOENT);
		retval = -1;
	}

	yaffsfs_Unlock();

	return retval;

}


int yaffs_chmod(const YCHAR *path, mode_t mode)
{
	yaffs_Object *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_FindObject(NULL,path,0);

	if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->myDev->readOnly)
		yaffsfs_SetError(-EINVAL);
	else
		retVal = yaffsfs_DoChMod(obj,mode);

	yaffsfs_Unlock();

	return retVal;

}


int yaffs_fchmod(int fd, mode_t mode)
{
	yaffs_Object *obj;

	int retVal = -1;

	yaffsfs_Lock();
	obj = yaffsfs_GetHandleObject(fd);

	if(!obj)
		yaffsfs_SetError(-ENOENT);
	else if(obj->myDev->readOnly)
		yaffsfs_SetError(-EINVAL);
	else
		retVal = yaffsfs_DoChMod(obj,mode);

	yaffsfs_Unlock();

	return retVal;
}


int yaffs_mkdir(const YCHAR *path, mode_t mode)
{
	yaffs_Object *parent = NULL;
	yaffs_Object *dir = NULL;
	YCHAR *name;
	int retVal= -1;

	yaffsfs_Lock();
	parent = yaffsfs_FindDirectory(NULL,path,&name,0);
	if(parent && parent->myDev->readOnly){
		yaffsfs_SetError(-EINVAL);
	} else {
		if(parent)
			dir = yaffs_MknodDirectory(parent,name,mode,0,0);
		if(dir)
			retVal = 0;
		else {
			if(!parent)
				yaffsfs_SetError(-ENOENT); // missing path
			else if (yaffs_FindObjectByName(parent,name))
				yaffsfs_SetError(-EEXIST); // the name already exists
			else
				yaffsfs_SetError(-ENOSPC); // just assume no space
			retVal = -1;
		}
	}

	yaffsfs_Unlock();

	return retVal;
}

int yaffs_mount2(const YCHAR *path,int readOnly)
{
	int retVal=-1;
	int result=YAFFS_FAIL;
	yaffs_Device *dev=NULL;
	YCHAR *dummy;

	T(YAFFS_TRACE_ALWAYS,(TSTR("yaffs: Mounting %s" TENDSTR),path));

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev){
		if(!dev->isMounted){
			dev->readOnly = readOnly ? 1 : 0;
			result = yaffs_GutsInitialise(dev);
			if(result == YAFFS_FAIL)
				// todo error - mount failed
				yaffsfs_SetError(-ENOMEM);
			retVal = result ? 0 : -1;

		}
		else
			//todo error - already mounted.
			yaffsfs_SetError(-EBUSY);
	} else
		// todo error - no device
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
        yaffs_Device *dev=NULL;
        YCHAR *dummy;
        
        yaffsfs_Lock();
        dev = yaffsfs_FindDevice(path,&dummy);
        if(dev){
                if(dev->isMounted){
                        
                        yaffs_FlushEntireDeviceCache(dev);
                        yaffs_CheckpointSave(dev);
                        
                        
                } else
                        //todo error - not mounted.
                        yaffsfs_SetError(-EINVAL);
                        
        }else
                // todo error - no device
                yaffsfs_SetError(-ENODEV);

        yaffsfs_Unlock();
        return retVal;  
}


int yaffs_remount(const YCHAR *path, int force, int readOnly)
{
        int retVal=-1;
	yaffs_Device *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev){
		if(dev->isMounted){
			int i;
			int inUse;

			yaffs_FlushEntireDeviceCache(dev);

			for(i = inUse = 0; i < YAFFSFS_N_HANDLES && !inUse && !force; i++){
				if(yaffsfs_handle[i].useCount>0 && yaffsfs_inode[yaffsfs_handle[i].inodeId].iObj->myDev == dev)
					inUse = 1; // the device is in use, can't unmount
			}

			if(!inUse || force){
				if(readOnly)
					yaffs_CheckpointSave(dev);
				dev->readOnly =  readOnly ? 1 : 0;
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
	yaffs_Device *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev){
		if(dev->isMounted){
			int i;
			int inUse;

			yaffs_FlushEntireDeviceCache(dev);
			yaffs_CheckpointSave(dev);

			for(i = inUse = 0; i < YAFFSFS_N_HANDLES && !inUse; i++){
				if(yaffsfs_handle[i].useCount > 0 && yaffsfs_inode[yaffsfs_handle[i].inodeId].iObj->myDev == dev)
					inUse = 1; // the device is in use, can't unmount
			}

			if(!inUse || force){
				yaffs_Deinitialise(dev);

				retVal = 0;
			} else
				// todo error can't unmount as files are open
				yaffsfs_SetError(-EBUSY);

		} else
			//todo error - not mounted.
			yaffsfs_SetError(-EINVAL);

	}
	else
		// todo error - no device
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
	yaffs_Device *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev  && dev->isMounted){
		retVal = yaffs_GetNumberOfFreeChunks(dev);
		retVal *= dev->nDataBytesPerChunk;

	} else
		yaffsfs_SetError(-EINVAL);

	yaffsfs_Unlock();
	return retVal;
}

loff_t yaffs_totalspace(const YCHAR *path)
{
	loff_t retVal=-1;
	yaffs_Device *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev  && dev->isMounted){
		retVal = (dev->endBlock - dev->startBlock + 1) - dev->nReservedBlocks;
		retVal *= dev->nChunksPerBlock;
		retVal *= dev->nDataBytesPerChunk;

	} else
		yaffsfs_SetError(-EINVAL);

	yaffsfs_Unlock();
	return retVal;
}

int yaffs_inodecount(const YCHAR *path)
{
	loff_t retVal= -1;
	yaffs_Device *dev=NULL;
	YCHAR *dummy;

	yaffsfs_Lock();
	dev = yaffsfs_FindDevice(path,&dummy);
	if(dev  && dev->isMounted) {
	   int nObjects = dev->nObjectsCreated - dev->nFreeObjects;
	   if(nObjects > dev->nHardLinks)
		retVal = nObjects - dev->nHardLinks;
	}
	
	if(retVal < 0)
		yaffsfs_SetError(-EINVAL);
	
	yaffsfs_Unlock();
	return retVal;	
}



void yaffs_initialise(yaffsfs_DeviceConfiguration *cfgList)
{

	yaffsfs_DeviceConfiguration *cfg;

	yaffsfs_configurationList = cfgList;

	yaffsfs_InitHandles();

	cfg = yaffsfs_configurationList;

	while(cfg && cfg->prefix && cfg->dev){
		cfg->dev->isMounted = 0;
		cfg->dev->removeObjectCallback = yaffsfs_RemoveObjectCallback;
		cfg++;
	}


}


//
// Directory search stuff.

//
// Directory search context
//
// NB this is an opaque structure.


typedef struct
{
	__u32 magic;
	yaffs_dirent de;		/* directory entry being used by this dsc */
	YCHAR name[NAME_MAX+1];		/* name of directory being searched */
        yaffs_Object *dirObj;           /* ptr to directory being searched */
        yaffs_Object *nextReturn;       /* obj to be returned by next readddir */
        int offset;
        struct ylist_head others;       
} yaffsfs_DirectorySearchContext;



static struct ylist_head search_contexts;


static void yaffsfs_SetDirRewound(yaffsfs_DirectorySearchContext *dsc)
{
	if(dsc &&
	   dsc->dirObj &&
	   dsc->dirObj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY){

           dsc->offset = 0;

           if( ylist_empty(&dsc->dirObj->variant.directoryVariant.children))
                dsc->nextReturn = NULL;
           else
                dsc->nextReturn = ylist_entry(dsc->dirObj->variant.directoryVariant.children.next,
                                                yaffs_Object,siblings);
        } else {
		/* Hey someone isn't playing nice! */
	}
}

static void yaffsfs_DirAdvance(yaffsfs_DirectorySearchContext *dsc)
{
	if(dsc &&
	   dsc->dirObj &&
           dsc->dirObj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY){

           if( dsc->nextReturn == NULL ||
               ylist_empty(&dsc->dirObj->variant.directoryVariant.children))
                dsc->nextReturn = NULL;
           else {
                   struct ylist_head *next = dsc->nextReturn->siblings.next;

                   if( next == &dsc->dirObj->variant.directoryVariant.children)
                        dsc->nextReturn = NULL; /* end of list */
                   else
                        dsc->nextReturn = ylist_entry(next,yaffs_Object,siblings);
           }
        } else {
                /* Hey someone isn't playing nice! */
	}
}

static void yaffsfs_RemoveObjectCallback(yaffs_Object *obj)
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
 	yaffs_Object *obj = NULL;
	yaffsfs_DirectorySearchContext *dsc = NULL;

	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,dirname,0);

	if(obj && obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY){

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
			dsc->de.d_ino = yaffs_GetEquivalentObject(dsc->nextReturn)->objectId;
			dsc->de.d_dont_use = (unsigned)dsc->nextReturn;
			dsc->de.d_off = dsc->offset++;
			yaffs_GetObjectName(dsc->nextReturn,dsc->de.d_name,NAME_MAX);
			if(yaffs_strnlen(dsc->de.d_name,NAME_MAX+1) == 0)
			{
				// this should not happen!
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

// end of directory stuff


int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath)
{
	yaffs_Object *parent = NULL;
	yaffs_Object *obj;
	YCHAR *name;
	int retVal= -1;
	int mode = 0; // ignore for now

	yaffsfs_Lock();
	parent = yaffsfs_FindDirectory(NULL,newpath,&name,0);
	if(parent && parent->myDev->readOnly)
		yaffsfs_SetError(-EINVAL);
	else if(parent){
		obj = yaffs_MknodSymLink(parent,name,mode,0,0,oldpath);
		if(obj)
			retVal = 0;
		else{
			yaffsfs_SetError(-ENOSPC); // just assume no space for now
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
	yaffs_Object *obj = NULL;
	int retVal;


	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,path,0);

	if(!obj) {
		yaffsfs_SetError(-ENOENT);
		retVal = -1;
	} else if(obj->variantType != YAFFS_OBJECT_TYPE_SYMLINK) {
		yaffsfs_SetError(-EINVAL);
		retVal = -1;
	} else {
		YCHAR *alias = obj->variant.symLinkVariant.alias;
		memset(buf,0,bufsiz);
		yaffs_strncpy(buf,alias,bufsiz - 1);
		retVal = 0;
	}
	yaffsfs_Unlock();
	return retVal;
}

int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath)
{
	// Creates a link called newpath to existing oldpath
	yaffs_Object *obj = NULL;
	yaffs_Object *target = NULL;
	int retVal = 0;
	int newNameLength = 0;


	yaffsfs_Lock();

	obj = yaffsfs_FindObject(NULL,oldpath,0);
	target = yaffsfs_FindObject(NULL,newpath,0);

	if(!obj) {
		yaffsfs_SetError(-ENOENT);
		retVal = -1;
	} else if(obj->myDev->readOnly){
		yaffsfs_SetError(-EINVAL);
		retVal= -1;
	} else if(target) {
		yaffsfs_SetError(-EEXIST);
		retVal = -1;
	} else {
		yaffs_Object *newdir = NULL;
		yaffs_Object *link = NULL;

		YCHAR *newname;

		newdir = yaffsfs_FindDirectory(NULL,newpath,&newname,0);

		if(!newdir){
			yaffsfs_SetError(-ENOTDIR);
			retVal = -1;
		}else if(newdir->myDev != obj->myDev){
			yaffsfs_SetError(-EXDEV);
			retVal = -1;
		}
		
		newNameLength = yaffs_strnlen(newname,YAFFS_MAX_NAME_LENGTH+1);
		
		if(newNameLength == 0){
			yaffsfs_SetError(-ENOENT);
			retVal = -1;
		} else if (newNameLength > YAFFS_MAX_NAME_LENGTH){
			yaffsfs_SetError(-ENAMETOOLONG);
			retVal = -1;
		}
		
		if(retVal == 0) {
			link = yaffs_Link(newdir,newname,obj);
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

int yaffs_DumpDevStruct(const YCHAR *path)
{
#if 0
	YCHAR *rest;

	yaffs_Object *obj = yaffsfs_FindRoot(path,&rest);

	if(obj){
		yaffs_Device *dev = obj->myDev;

		printf("\n"
			   "nPageWrites.......... %d\n"
			   "nPageReads........... %d\n"
			   "nBlockErasures....... %d\n"
			   "nGCCopies............ %d\n"
			   "garbageCollections... %d\n"
			   "passiveGarbageColl'ns %d\n"
			   "\n",
				dev->nPageWrites,
				dev->nPageReads,
				dev->nBlockErasures,
				dev->nGCCopies,
				dev->garbageCollections,
				dev->passiveGarbageCollections
		);

	}

#endif
	return 0;
}

