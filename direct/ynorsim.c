

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

static __u32 word[YNORSIM_DEV_SIZE_U32];

extern int random_seed;
extern int simulate_power_failure;

static void NorError(void)
{
  printf("Nor error\n");
  while(1){}
}

static void ynorsim_SaveImage(void)
{
  int h = open(YNORSIM_FNAME, O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
  write(h,word,sizeof(word));
  close(h);
}

static void ynorsim_RestoreImage(void)
{
  int h = open(YNORSIM_FNAME, O_RDONLY, S_IREAD | S_IWRITE);
  memset(word,0xFF,sizeof(word));
  read(h,word,sizeof(word));
  close(h);
}


static void ynorsim_PowerFail(void)
{
  ynorsim_SaveImage();
  exit(1);
}

static int initialised = 0;
static int remaining_ops;
static int nops_so_far;

int ops_multiplier = 500;

static void ynorsim_MaybePowerFail(void)
{

   nops_so_far++;
   
   
   remaining_ops--;
   if(simulate_power_failure &&
      remaining_ops < 1){
       printf("Simulated power failure after %d operations\n",nops_so_far);
    	ynorsim_PowerFail();
  }
}

static void ynorsim_Ready(void)
{
  if(initialised) 
    return;
  srand(random_seed);
  remaining_ops = 1000000000;
  remaining_ops = (rand() % 10000) * ops_multiplier * YNORSIM_BIT_CHANGES;
  ynorsim_RestoreImage();
}

void ynorsim_Read32(__u32 *addr,__u32 *buf, int nwords)
{ 
   while(nwords > 0){
     *buf = *addr;
     buf++;
     addr++;
     nwords--;
   }
}

void ynorsim_WriteOneWord32(__u32 *addr,__u32 val)
{
  __u32 tmp;
  __u32 m;
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
      ynorsim_MaybePowerFail();
    }
       
  }
  
  *addr = tmp & val;
  ynorsim_MaybePowerFail();
}

void ynorsim_Write32(__u32 *addr, __u32 *buf, int nwords)
{
  while(nwords >0){
    ynorsim_WriteOneWord32(addr,*buf);
    addr++;
    buf++;
    nwords--;
  }
}

void ynorsim_EraseBlock(__u32 *addr)
{
  /* Todo... bit flipping */
  memset(addr,0xFF,YNORSIM_BLOCK_SIZE_U32 * 4);
}

void ynorsim_Initialise(void)
{
  ynorsim_Ready();
}

void ynorsim_Shutdown(void)
{
  ynorsim_SaveImage();
  initialised=0;
}

__u32 *ynorsim_GetBase(void)
{
  return word;
}
