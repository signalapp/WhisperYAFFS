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

#include "ynorsim.h"



#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define YNORSIM_FNAME "emfile-nor"

/* Set YNORSIM_BIT_CHANGES to a a value from 1..30 to 
 *simulate bit flipping as the programming happens. 
 * A low value results in faster simulation with less chance of encountering a partially programmed
 * word. 
 */
   
//#define YNORSIM_BIT_CHANGES 15
#define YNORSIM_BIT_CHANGES 2

#if 0
/* Simulate 32MB of flash in 256k byte blocks.
 * This stuff is x32.
 */

#define YNORSIM_BLOCK_SIZE_U32  (256*1024/4)
#define YNORSIM_DEV_SIZE_U32	(32*1024 * 1024/4)
#else
/* Simulate 8MB of flash in 256k byte blocks.
 * This stuff is x32.
 */

#define YNORSIM_BLOCK_SIZE_U32  (256*1024/4)
#define YNORSIM_DEV_SIZE_U32	(8*1024 * 1024/4)
#endif

static u32 word[YNORSIM_DEV_SIZE_U32];

extern int random_seed;
extern int simulate_power_failure;

static void NorError(void)
{
  printf("Nor error\n");
  while(1){}
}

static void ynorsim_save_image(void)
{
  int h = open(YNORSIM_FNAME, O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
  write(h,word,sizeof(word));
  close(h);
}

static void ynorsim_restore_image(void)
{
  int h = open(YNORSIM_FNAME, O_RDONLY, S_IREAD | S_IWRITE);
  memset(word,0xFF,sizeof(word));
  read(h,word,sizeof(word));
  close(h);
}


static void ynorsim_power_fail(void)
{
  ynorsim_save_image();
  exit(1);
}

static int initialised = 0;
static int remaining_ops;
static int nops_so_far;

int ops_multiplier = 500;

static void ynorsim_maybe_power_fail(void)
{

   nops_so_far++;
   
   
   remaining_ops--;
   if(simulate_power_failure &&
      remaining_ops < 1){
       printf("Simulated power failure after %d operations\n",nops_so_far);
    	ynorsim_power_fail();
  }
}

static void ynorsim_ready(void)
{
  if(initialised) 
    return;
  srand(random_seed);
  remaining_ops = 1000000000;
  remaining_ops = (rand() % 10000) * ops_multiplier * YNORSIM_BIT_CHANGES;
  ynorsim_restore_image();
}

void ynorsim_rd32(u32 *addr,u32 *buf, int nwords)
{ 
   while(nwords > 0){
     *buf = *addr;
     buf++;
     addr++;
     nwords--;
   }
}

void ynorsim_wr_one_word32(u32 *addr,u32 val)
{
  u32 tmp;
  u32 m;
  int i;

  tmp = *addr;
  if(val & ~tmp){
    // Fail due to trying to change a zero into a 1
    printf("attempt to set a zero to one (%x)->(%x)\n",tmp,val);
    NorError();
  }
  
  for(i = 0; i < YNORSIM_BIT_CHANGES; i++){
    m = 1 << (rand() & 31);
    if(!(m & val)){
      tmp &= ~m;
      *addr = tmp;
      ynorsim_maybe_power_fail();
    }
       
  }
  
  *addr = tmp & val;
  ynorsim_maybe_power_fail();
}

void ynorsim_wr32(u32 *addr, u32 *buf, int nwords)
{
  while(nwords >0){
    ynorsim_wr_one_word32(addr,*buf);
    addr++;
    buf++;
    nwords--;
  }
}

void ynorsim_erase(u32 *addr)
{
  /* Todo... bit flipping */
  memset(addr,0xFF,YNORSIM_BLOCK_SIZE_U32 * 4);
}

void ynorsim_initialise(void)
{
  ynorsim_ready();
}

void ynorsim_shutdown(void)
{
  ynorsim_save_image();
  initialised=0;
}

u32 *ynorsim_get_base(void)
{
  return word;
}
