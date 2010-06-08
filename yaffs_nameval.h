#ifndef __NAMEVAL_H__
#define __NAMEVAL_H__

#define NVAL_CREATE	0x01
#define NVAL_REPLACE	0x02

int nval_del(char *xb, int xb_size, const char *name);
int nval_set(char *xb, int xb_size, const char *name, const char *buf, int bsize, int flags);
int nval_get(const char *xb, int xb_size, const char *name, char *buf, int bsize);
int nval_list(const char *xb, int xb_size, char *buf, int bsize);
int nval_load(char *xb, int xb_size, const char *src, int src_size);
int nval_save(const char *xb, int xb_size, char *dest, int dest_size);

#endif
