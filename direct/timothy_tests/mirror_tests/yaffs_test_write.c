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

#include "yaffs_test_write.h"

int yaffs_test_write(arg_temp *args_struct)
{
	char path[200];
	char message[200];
	int output=0,output2=0;
	int handle;
	struct yaffs_stat yaffs_stat_struct;
	join_paths(yaffs_struct.root_path,args_struct->string1, path );
	sprintf(message,"trying to write to: %s\nwith mode set to %o \n",path,args_struct->char1);
	print_message(3,message);
	printf("mode S_IREAD %d S_IWRITE %d\n",(args_struct->char2 & S_IREAD),(args_struct->char2 & S_IWRITE));
	handle=yaffs_open(path,((args_struct->char1 &(O_TRUNC|O_EXCL|O_CREAT|O_APPEND))|O_RDWR),(args_struct->char2&(S_IREAD|S_IWRITE)));
	printf("handle %d\n",handle);
	if (handle<0){
		print_message(3,"failed to open a handle\n");
		return -1; //handle failed to open
	}

/*	output=yaffs_fstat(handle,&yaffs_stat_struct);
	if (output < 0){
		sprintf(message,"failed to stat the file\n");
		print_message(3,message);
		get_error_yaffs();
	} else {
		sprintf(message,"stated the file\n");
		print_message(3,message);
		sprintf(message," yaffs file mode is %d\n",(yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE)));
		print_message(3,message);
		sprintf(message,"mode S_IREAD %d S_IWRITE %d\n",(yaffs_stat_struct.st_mode & S_IREAD),(yaffs_stat_struct.st_mode & S_IWRITE));
		print_message(3,message);	
	}

	sprintf(message,"trying to write: %d bytes into the file\n",strlen(args_struct->string2));
	print_message(3,message);
*/	
	output=yaffs_write(handle,&args_struct->string2,strlen(args_struct->string2));

/*	print_message(3,"\n wrote to the file\n");
	output2=yaffs_fstat(handle,&yaffs_stat_struct);
	if (output2 < 0){
		sprintf(message,"failed to stat the file\n");
		print_message(3,message);
		get_error_yaffs();
	} else {
		sprintf(message,"stated the file\n");
		print_message(3,message);
		sprintf(message," yaffs file mode is %d\n",(yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE)));
		print_message(3,message);
		sprintf(message,"mode S_IREAD %d S_IWRITE %d\n",(yaffs_stat_struct.st_mode & S_IREAD),(yaffs_stat_struct.st_mode & S_IWRITE));
		print_message(3,message);	
	}
*/	yaffs_close(handle);
/*	print_message(3,"\n closing the file\n");
	output2=yaffs_stat(path,&yaffs_stat_struct);
	if (output2 < 0){
		sprintf(message,"failed to stat the file\n");
		print_message(3,message);
		get_error_yaffs();
	} else {
		sprintf(message,"stated the file\n");
		print_message(3,message);
		sprintf(message," yaffs file mode is %d\n",(yaffs_stat_struct.st_mode & (S_IREAD| S_IWRITE)));
		print_message(3,message);
		sprintf(message,"mode S_IREAD %d S_IWRITE %d\n",(yaffs_stat_struct.st_mode & S_IREAD),(yaffs_stat_struct.st_mode & S_IWRITE));
		print_message(3,message);	
	}
*/

	return output;
}
