/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include "nor_stress.h"


#include "yaffsfs.h"
#include "yaffs_fsx.h"

#include <stdio.h>


extern int fuzz_test;

#if 1
#define FSX_INIT(mount_pt) do{ if(interleave_fsx) yaffs_fsx_init(mount_pt); } while(0)
#define FSX_COMPLETE() do { if(interleave_fsx) yaffs_fsx_complete(); } while (0)

#define FSX() \
do { \
  if((myrand() & 0x1) == 0){\
     if(interleave_fsx) \
        yaffs_fsx_do_op(); \
  } \
} while(0)

#else
#define FSX_INIT(mount_point) do { } while(0)
#define FSX_COMPLETE() do { } while(0)
#define FSX() do { } while(0)
#endif


void (*ext_fatal)(void) = NULL;

static unsigned powerUps;
static unsigned cycleStarts;
static unsigned cycleEnds;

static int interleave_fsx;

static int no_verification;
 
char fullPathName[100];
char fullPowerUpName[100];
char fullStartName[100];
char fullEndName[100];
char fullMainName[100];
char fullTempMainName[100];
char fullTempCounterName[100];


extern int random_seed;

int myrand(void) {
  random_seed = random_seed * 1103515245 + 12345;
  return((unsigned)(random_seed/65536) % 32768);
}

void MakeName(char *fullName,const char *prefix, const char *name)
{
  strcpy(fullName,prefix);
  strcat(fullName,"/");
  strcat(fullName,name);
}


void MakeFullNames(const char *prefix)
{
  MakeName(fullPathName,prefix,"");
  MakeName(fullPowerUpName,prefix,"powerUps");
  MakeName(fullStartName,prefix,"starts");
  MakeName(fullEndName,prefix,"ends");
  MakeName(fullMainName,prefix,"main");
  MakeName(fullTempCounterName,prefix,"tmp-counter");
  MakeName(fullTempMainName,prefix,"tmp-main");
}

static void FatalError(int lineNo)
{
  printf("Integrity error %d\n",lineNo);

  if(fuzz_test)
  	return;

  if(ext_fatal)
  	ext_fatal();
  	
  while(1){
   sleep(1);
  }
}
void print_stat(const char *str, struct yaffs_stat *st)
{
	printf("%s inode %d\n",str,st->st_ino);
}

static void UpdateCounter(const char *name, unsigned *val,  int initialise)
{
  int inh=-1;
  int outh=-1;
  unsigned x[2];
  int nread = 0;
  int nwritten = 0;
  
  x[0] = x[1] = 0;
  
  if(initialise){
    x[0] = 0; 
    x[1] = 1;
  } else {
    inh = yaffs_open(name,O_RDONLY, S_IREAD | S_IWRITE);
    if(inh >= 0){
      nread = yaffs_read(inh,x,sizeof(x));
      yaffs_close(inh);
    }

    if(!no_verification &&
      (nread != sizeof(x) ||
       x[0] + 1 != x[1])){
      printf("Error reading counter %s handle %d, x[0] %u x[1] %u last error %d\n",
              name, inh, x[0], x[1],yaffsfs_GetLastError());
      FatalError(__LINE__);
              
    }
    x[0]++;
    x[1]++;
  }
  
  FSX();
  outh = yaffs_open(fullTempCounterName, O_RDWR | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
  
  if(outh >= 0){
   struct yaffs_stat tmpstat, oldstat, tmpfstat;
   FSX(); 
    yaffs_fstat(outh,&tmpfstat);
    printf("\n\n\n*** Writing file %s inode %d\n",fullTempCounterName,tmpfstat.st_ino);
    nwritten = yaffs_write(outh,x,sizeof(x));
    FSX();
    yaffs_close(outh);
    FSX();

    printf("About to rename %s to %s\n",fullTempCounterName,name);
    yaffs_stat(fullTempCounterName,&tmpstat);
    yaffs_stat(name,&oldstat);
    print_stat("old stat",&oldstat);
    print_stat("new stat",&tmpstat);
    print_stat("new fstat",&tmpfstat);
    yaffs_rename(fullTempCounterName,name);
    FSX();
  }
  
  if(nwritten != sizeof(x)){
      printf("Error writing counter %s handle %d, x[0] %u x[1] %u\n",
              name, inh, x[0], x[1]);
      FatalError(__LINE__);
  }
  
  *val = x[0];
  
  printf("##\n"
         "## Set counter %s to %u\n"
         "##\n", name,x[0]);
}


static void dump_directory_tree_worker(const char *dname,int recursive)
{
	yaffs_DIR *d;
	yaffs_dirent *de;
	struct yaffs_stat s;
	char str[1000];
	int error_line = 0;
	int nentries;
			
	d = yaffs_opendir(dname);
	
	if(!d)
	{
		printf("opendir failed\n");
	}
	else
	{
		nentries = 0;
		while((de = yaffs_readdir(d)) != NULL)
		{
			strcpy(str,dname);
			strcat(str,"/");
			strcat(str,de->d_name);
			nentries++;
			
			yaffs_lstat(str,&s);
			
			printf("%s inode %ld %d obj %x length %d mode %X ",str, de->d_ino, s.st_ino,de->d_dont_use,(int)s.st_size,s.st_mode);\
			if(de->d_ino != s.st_ino){
				printf(" \n\n!!!! HEY inode mismatch\n\n");
				error_line = __LINE__;
			}

			switch(s.st_mode & S_IFMT)
			{
				case S_IFREG: printf("data file"); break;
				case S_IFDIR: printf("directory"); break;
				case S_IFLNK: printf("symlink -->");
							  if(yaffs_readlink(str,str,100) < 0)
								printf("no alias");
							  else
								printf("\"%s\"",str);    
							  break;
				default: printf("unknown"); break;
			}
			
			printf("\n");

			if((s.st_mode & S_IFMT) == S_IFDIR && recursive)
				dump_directory_tree_worker(str,1);
				
                        if(s.st_ino > 10000)
                        	error_line = __LINE__;
							
		}
		
		if(strstr(dname,"lost+found") && nentries >0){
			printf("\n\n!!! HEY lost+found not empty, had %d entries\n\n\n",nentries);
			error_line = __LINE__;
		}

		if(error_line && !no_verification)
			FatalError(error_line);
		
		yaffs_closedir(d);
	}

}

static void dump_directory_tree(const char *dname)
{
	dump_directory_tree_worker(dname,1);
	printf("\n");
	printf("Free space in %s is %d\n\n",dname,(int)yaffs_freespace(dname));
}




#define XX_SIZE 500

static unsigned xx[XX_SIZE];

static int yWriteFile(const char *fname, unsigned sz32)
{
	int h;
	int r;
	int i;
	struct yaffs_stat st;
	unsigned checksum = 0;
	

	FSX();
	h = yaffs_open(fname,O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
	yaffs_fstat(h,&st);
	printf("\n\n\n**** Open writing file %s inode %d\n",fname, st.st_ino);
	
	FSX();

	if(h < 0){
		printf("could not open file %s\n",fname);
		return h;
	}

        xx[0] = sz32;
        checksum ^= xx[0];

	if((r = yaffs_write(h,xx,sizeof(unsigned))) != sizeof(unsigned)){
		goto WRITE_ERROR;
	}
        FSX();
	while(sz32> 0){
        	for(i = 0; i < XX_SIZE; i++){
        	  xx[i] = sz32 + i;
        	  checksum ^= xx[i];
                }
                
                FSX();
		if((r = yaffs_write(h,xx,sizeof(xx))) != sizeof(xx)){
			goto WRITE_ERROR;
		}
		sz32--;
	}

	xx[0] = checksum;
	FSX();
	if((r = yaffs_write(h,xx,sizeof(unsigned))) != sizeof(unsigned)){
		goto WRITE_ERROR;
	}
	
	FSX();
	yaffs_close(h);
	printf("File closed\n");
	return 0;

WRITE_ERROR:
	printf("ywrite error at position %d\n",(int)yaffs_lseek(h,0,SEEK_END));
	yaffs_close(h);
	return -1;
	
}

static int yVerifyFile(const char *fName)
{
	unsigned checksum = 0;
	unsigned totalSize;
	unsigned sz32;
	unsigned recordedSize = 0;
	int r;
	int h;
	int i;
	int retval = 0;

	if(no_verification)
		return 0;

        printf("Verifying file %s\n",fName);
        	
	h = yaffs_open(fName, O_RDONLY,S_IREAD | S_IWRITE);

	if(h < 0){
		printf("could not open file %s\n",fName);
		return -1;
	}

	totalSize = yaffs_lseek(h,0,SEEK_END);
	yaffs_lseek(h,0,SEEK_SET);

	r = yaffs_read(h,&sz32,sizeof(sz32));

	if(r != sizeof(sz32)){
		printf("reading size failed ... returned %d\n",r);
		yaffs_close(h);
		return -1;
	}
	
	recordedSize = sz32 * sizeof(xx) + 8;

	printf("verify %s: file size is %d, recorded size is %d\n", fName, totalSize, recordedSize);
	if(totalSize != recordedSize){
		printf("!!!!!!!!!!!!!!!!!!!!!!!!file size is wrong, should be %d, is %d\n", recordedSize,totalSize);
		yaffs_close(h);
		return -1;
	}

	checksum ^= sz32;


	while(sz32 > 0){
		r = yaffs_read(h,xx,sizeof(xx));
		if(r != sizeof(xx)){
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!reading data failed ... returned %d\n",r);
			yaffs_close(h);
			return -1;
		}
		for(i = 0; i < XX_SIZE; i++)
		  checksum ^= xx[i];
		sz32--;
	}
	r = yaffs_read(h,xx,sizeof(xx[0]));
	if(r != sizeof(xx[0])){
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!reading data failed ... returned %d\n",r);
		yaffs_close(h);
		return -1;
	}
	
	checksum ^= xx[0];

	if(checksum != 0){
		printf("!!!!!!!!!!!!!!!!!!!!! checksum failed\n");
		retval = -1;
        } else
		printf("verified ok\n");
	yaffs_close(h);

	return retval;
}


static void DoUpdateMainFile(void)
{
        int result;
        int sz32;
        sz32 = (myrand() % 1000)   + 20;
        
	result = yWriteFile(fullTempMainName,sz32);
	FSX();
	if(!no_verification && result)
	    FatalError(__LINE__);
	printf("Raname file %s to %s\n",fullTempMainName,fullMainName);
	yaffs_rename(fullTempMainName,fullMainName);
	FSX();
}

static void DoVerifyMainFile(void)
{
        int result;
	if(no_verification)
		return;
	result = yVerifyFile(fullMainName);
	if(result)
	    FatalError(__LINE__);

}


void NorStressTestInitialise(const char *prefix)
{
  MakeFullNames(prefix);
  
  UpdateCounter(fullPowerUpName,&powerUps,1);
  UpdateCounter(fullStartName,&cycleStarts,1);
  UpdateCounter(fullEndName,&cycleEnds,1);
  UpdateCounter(fullPowerUpName,&powerUps,1);
  DoUpdateMainFile();
  DoVerifyMainFile();
}


void NorStressTestRun(const char *prefix, int n_cycles, int do_fsx, int skip_verification)
{

  interleave_fsx = do_fsx;
  no_verification = skip_verification;
 
  MakeFullNames(prefix);
  dump_directory_tree(fullPathName);
  FSX_INIT(prefix);
    
  dump_directory_tree(fullPathName);
  
  UpdateCounter(fullPowerUpName,&powerUps,0);
  dump_directory_tree(fullPathName);
  
  while(n_cycles < 0 || n_cycles > 0){
    if(n_cycles > 0)
      n_cycles--;
    UpdateCounter(fullStartName, &cycleStarts,0);
    dump_directory_tree(fullPathName);
    DoVerifyMainFile();
    DoUpdateMainFile();
    dump_directory_tree(fullPathName);
  
    UpdateCounter(fullEndName,&cycleEnds,0);
    dump_directory_tree(fullPathName);
  }
  FSX_COMPLETE();
}
