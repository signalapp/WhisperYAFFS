/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Timothy Manning <timothy@yaffs.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "linux_test_write.h"

int linux_test_write(arg_temp *args_struct)
{
	char path[200];
	char message[200];
	int output=0;
	int handle=-1;
	join_paths(linux_struct.root_path,args_struct->string1, path );
	sprintf(message,"trying to write to: %s\nwith mode set to %o \n",path,args_struct->char1);
	print_message(3,message);
	handle=open(path,args_struct->char1 &(O_TRUNC|O_EXCL|O_CREAT|O_APPEND),args_struct->char2&(S_IREAD|S_IWRITE));
	if (handle<0){
		print_message(3,"failed to open a handle\n");
		return -1; //handle failed to open
	}
	sprintf(message,"trying to write: %d bytes into the file\n",strlen(args_struct->string2));
	print_message(3,message);
	output=write(handle,args_struct->string2,strlen(args_struct->string2));
	close(handle);
	return output;
}
