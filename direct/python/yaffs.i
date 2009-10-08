%module yaffs
%{
#include "yaffsfs_for_swig.h"
void yaffs_print_constants(void);
%}

%include "yaffsfs_for_swig.h"
void yaffs_print_constants(void);
