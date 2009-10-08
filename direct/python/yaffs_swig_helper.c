/*
 * These are some dangly bits that need to be built to wrap up the rest of the yaffs test harness
 * in SWIG.
 *
 * This is also the place where extra debugging stuff might end up.
 *
 */
 
#include "yaffsfs.h"

int simulate_power_failure;
int random_seed;

int yaffs_print_constants(void)
{
	printf( "O_CREAT........%d\n",O_CREAT);
	printf( "O_RDONLY.......%d\n",O_RDONLY);
	printf( "O_WRONLY.......%d\n",O_WRONLY);
	printf( "O_RDWR.........%d\n",O_RDWR);
	printf( "O_TRUNC........%d\n",O_TRUNC);

	return 0;
}
