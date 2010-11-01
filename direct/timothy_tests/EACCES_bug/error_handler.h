/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __error_handler_h__
#define __error_handler_h__
#include <stdio.h>
#include "message_buffer.h"
#include "yaffsfs.h"
#define DEBUG_LEVEL 5 /*set the debug level. this is used to display the relevent debug messages*/
void yaffs_check_for_errors(int output, buffer *message_buffer, char error_message[], char success_message[]);
#endif
