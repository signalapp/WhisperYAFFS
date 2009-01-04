#ifndef __NOR_STRESS_H__
#define __NOR_STRESS_H__

void NorStressTestInitialise(const char *path);
void NorStressTestRun(const char *path, int n_cycles, int do_fsx);

#endif

