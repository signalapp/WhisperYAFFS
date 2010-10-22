/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>


static void print_xattrib_val(const char *path, const char *name)
{
	char buffer[100];
	int n;
	
	n = getxattr(path,name,buffer,sizeof(buffer));
	if(n >= 0){
		uint8_t *b = (uint8_t *)buffer;

		printf("%d bytes:",n);
		while(n > 0){
			printf("[%02X]",*b);
			b++;
			n--;
		}
		printf("\n");
	} else
		printf(" Novalue result %d\n",n);
}

static void list_xattr(const char *path)
{
	char list[1000];
	int n=0;
	int list_len;
	int len;
	
	list_len = listxattr(path,list,sizeof(list));
	printf("xattribs for %s, result is %d\n",path,list_len);
	while(n < list_len){
		len = strlen(list + n);
		printf("\"%s\" value ",list+n);
		print_xattrib_val(path,list + n);
		n += (len + 1);
	}
	printf("end\n");
	
}

void basic_xattr_test(const char *mountpt)
{
	char name[100];
	int h;
	int result;
	int val1;
	int valread;

	
	strcpy(name,mountpt);
	strcat(name,"/");
	strcat(name,"xfile");

	unlink(name);
	h = open(name,O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
	close(h);
	
	printf("Start\n");
	list_xattr(name);

	printf("Add an attribute\n");
	val1 = 0x123456;
	result = setxattr(name,"foo",&val1,sizeof(val1),0);
	printf("wrote attribute foo: result %d\n",result);
	list_xattr(name);
	printf("Add an attribute\n");
	val1 = 0x7890;
	result = setxattr(name,"bar",&val1,sizeof(val1),0);
	printf("wrote attribute bar: result %d\n",result);
	list_xattr(name);
	
	printf("Get non-existanrt attribute\n");
	print_xattrib_val(name,"not here");
	
	printf("Delete non existing attribute\n");
	removexattr(name,"not here");
	list_xattr(name);

	printf("Remove foo\n");
	removexattr(name,"foo");
	list_xattr(name);

	printf("Remove bar\n");
	removexattr(name,"bar");
	list_xattr(name);
	
}
	

int random_seed;
int simulate_power_failure;

int main(int argc, char *argv[])
{
	basic_xattr_test("/mnt/");	
}
