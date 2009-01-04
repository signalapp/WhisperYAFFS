#ifndef __YAFFS_FSX_H__
#define __YAFFS_FSX_H__

int yaffs_fsx_init(const char *mountpt);
int yaffs_fsx_complete(void);
int yaffs_fsx_do_op(void);

#endif

