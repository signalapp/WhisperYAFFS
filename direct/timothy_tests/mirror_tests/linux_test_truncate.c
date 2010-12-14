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

#include "linux_test_truncate.h"

int linux_test_truncate(arg_temp *args_struct)
{
	char path[200];
	char message[200];
	int output=0;
	join_paths(linux_struct.root_path,args_struct->string1, path );
	sprintf(message,"file path: %s\n",path);	
	print_message(3,message);

	output=truncate(path,args_struct->int1);
	return output;
}
