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

#include "linux_test_unlink.h"

int linux_test_unlink(arg_temp *args_struct)
{
	char path[250];
	char message[150];
	int output;
	join_paths(linux_struct.root_path,args_struct->string1, path );
	sprintf(message,"file path: %s\n",path);	
	print_message(3,message);

	output= unlink(path);
	
	if (output<0) {
		print_message(3,"failed to unlink file\n");
		return -1;
	} else {
		return 1;
	}
}
