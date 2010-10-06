
#ifndef __Y_NORSIM_H__
#define __Y_NORSIM_H__

#include "yaffs_guts.h"

void ynorsim_rd32(__u32 *addr, __u32 *data, int nwords);
void ynorsim_wr32(__u32 *addr, __u32 *data, int nwords);
void ynorsim_erase(__u32 *addr);
void ynorsim_shutdown(void);
void ynorsim_initialise(void);
__u32 * ynorsim_get_base(void);

#endif
