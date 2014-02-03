#ifndef CGSOLVE_H
#define CGSOLVE_H

void cgsolve_sequential(int k, double* result, double* norm, int* num_iter);
void cgsolve_parallel(int k, int rank, int size, double* result, double* norm, int* num_iter);

#endif