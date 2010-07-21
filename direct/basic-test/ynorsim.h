
#ifndef __Y_NORSIM_H__
#define __Y_NORSIM_H__

#include "yaffs_guts.h"

void ynorsim_Read32(__u32 *addr, __u32 *data, int nwords);
void ynorsim_Write32(__u32 *addr, __u32 *data, int nwords);
void ynorsim_EraseBlock(__u32 *addr);
void ynorsim_Shutdown(void);
void ynorsim_Initialise(void);
__u32 * ynorsim_GetBase(void);

#endif
