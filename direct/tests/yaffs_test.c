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




#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>

#include "yaffsfs.h"

#include "yaffs_trace.h"

#include "nor_stress.h"
#include "yaffs_fsx.h"

void (*ext_fatal)(void);


int random_seed;
int simulate_power_failure = 0;


int do_fsx;
int do_bash_around;

int init_test;
int do_upgrade;
int n_cycles = -1;
int fuzz_test=0;


int yaffs_test_maxMallocs;

extern int ops_multiplier;

char mount_point[200];

#define BASH_HANDLES 20
void yaffs_bash_around(const char *mountpt, int n_cycles)
{
	char name[200];
	char name1[200];
	int  h[BASH_HANDLES];
	int i;
	int op;
	int pos;
	int n;
	int n1;
	int start_op;
	int op_max = 99;
	
	int cycle = 0;
	
	sprintf(name,"%s/bashdir",mountpt);
	
	yaffs_mkdir(name,0666);

	for(i = 0; i < BASH_HANDLES; i++)
		h[i] = -1;
		
	while(n_cycles){
		if(cycle % 100 == 0){
			printf("CYCLE %8d mo %2d inodes %d space %d ",cycle,op_max,
				yaffs_inodecount(mountpt),(int)yaffs_freespace(mountpt));
			for(i = 0; i < BASH_HANDLES; i++)
				printf("%2d ",h[i]);
			printf("\n");
		}
		cycle++;

		if(n_cycles > 0)
			n_cycles--;
		i = rand() % BASH_HANDLES;
		op = rand() % op_max;
		pos = rand() & 20000000;
		n = rand() % 100;
		n1 = rand() % 100;
		
		sprintf(name,"%s/bashdir/xx%d",mountpt,n);
		sprintf(name1,"%s/bashdir/xx%d",mountpt,n1);

		start_op = op;
		
		op-=1;
		if(op < 0){
			if(h[i]>= 0){
				yaffs_close(h[i]);
				h[i] = -1;
			}
			continue;
		}
		op-=1;
		if(op < 0){
			if(h[i] < 0)
				h[i] = yaffs_open(name,O_CREAT| O_RDWR, 0666);
			continue;
		}

		op-=5;
		if(op< 0){
			yaffs_lseek(h[i],pos,SEEK_SET);
			yaffs_write(h[i],name,n);
			continue;
		}
		op-=1;
		if(op < 0){
			yaffs_unlink(name);
			continue;
		}
		op-=1;
		if(op < 0){
			yaffs_rename(name,name1);
			continue;
		}
		op-=1;
		if(op < 0){
			yaffs_mkdir(name,0666);
			continue;
		}
		op-=1;
		if(op < 0){
			yaffs_rmdir(name);
			continue;
		}
		
		op_max = start_op-op;
	}
		
	
}

void BadUsage(void)
{
	printf("Usage: yaffs_test [options] mountpoint\n");
	printf("options\n");
	printf(" f: do fsx testing\n");
	printf(" i: initialise for upgrade testing\n");
	printf(" l: multiply number of operations by 5\n");
	printf(" m: random max number of mallocs\n");
	printf(" n nnn: number of cycles\n");
	printf(" p: simulate power fail testing\n");
	printf(" s sss: set seed\n");
	printf(" u: do upgrade test\n");
	printf(" b: bash-about test\n");
	printf(" z: fuzz test - ignore verification errors\n");
	exit(2);
}

int sleep_exit = 0;
int sleep_time = 0;
void test_fatal(void)
{
	printf("fatal yaffs test pid %d sleeping\n",getpid());
	while(!sleep_exit){
		sleep(1);
		sleep_time++;
	}
	
	signal(SIGSEGV,SIG_IGN);
	signal(SIGBUS,SIG_IGN);
	signal(SIGABRT,SIG_IGN);
	sleep_exit = 0;
	
}

void bad_ptr_handler(int sig)
{
	printf("signal %d received\n",sig);
	test_fatal();
}

int main(int argc, char **argv)
{
	int ch;
	int random_mallocs=0;
	ext_fatal = test_fatal;

#if 1
	signal(SIGSEGV,bad_ptr_handler);
	signal(SIGBUS,bad_ptr_handler);
	signal(SIGABRT,bad_ptr_handler);
#endif	
	while ((ch = getopt(argc,argv, "bfilmn:ps:t:uz"))
	       != EOF)
		switch (ch) {
		case 's':
			random_seed = atoi(optarg);
			break;
		case 'p':
			simulate_power_failure =1;
			break;
		case 'i':
			init_test = 1;
			break;
		case 'b':
			do_bash_around = 1;
			break;
		case 'f':
			do_fsx = 1;
			break;
		case 'l':
			ops_multiplier *= 5;
			break;
		case 'u':
			do_upgrade = 1;
			break;
		case 'm':
			random_mallocs=1;
			break;
		case 'n':
			n_cycles = atoi(optarg);
			break;
		case 't':
			yaffs_traceMask = strtol(optarg,NULL,0);
			break;
		case 'z':fuzz_test=1;
			break;
		default:
			BadUsage();
			/* NOTREACHED */
		}
	argc -= optind;
	argv += optind;
	
	if(random_mallocs){
		yaffs_test_maxMallocs = 0xFFF & random_seed;
	}
	
	if(argc == 1) {
		int result;

		strcpy(mount_point,argv[0]);
		
		if(simulate_power_failure)
			n_cycles = -1;
		printf("Running test %s %s %s %s %s seed %d cycles %d\n",
			do_upgrade ? "fw_upgrade" : "",
			init_test ? "initialise":"",
			fuzz_test ? "fuzz-test" : "",
			do_fsx ? "fsx" :"",
			simulate_power_failure ? "power_fail" : "",
			random_seed, n_cycles);

		yaffs_StartUp();
		result = yaffs_mount(mount_point);
		if(result < 0){
			printf("Mount of %s failed\n",mount_point);
			printf("pid %d sleeping\n",getpid());
			
			while(!sleep_exit){
				sleep(1);
				sleep_time++;
			}
		}
		printf("Mount complete\n");
			
		if(do_upgrade && init_test){
			simulate_power_failure = 0;
			NorStressTestInitialise(mount_point);
		} else if(do_upgrade){
			printf("Running stress on %s with seed %d\n",mount_point,random_seed);
			NorStressTestRun(mount_point,n_cycles,do_fsx,fuzz_test);
		} else if(do_fsx){
			yaffs_fsx_main(mount_point,n_cycles);
		} else if(do_bash_around){
			yaffs_bash_around(mount_point,n_cycles);
		}else {
			printf("No test to run!\n");
			BadUsage();
		}
		yaffs_unmount(mount_point);
		
		printf("Test run completed!\n");
	}
	else
		BadUsage();
	return 0;
}


