#include<stdio.h>
#include<assert.h>
#include "daxpy.h"

// #define DEBUG_1

void daxpy(double alpha, double* a, double beta, double* b, int n, double* result) {

  int i;  
  assert(n>0);
#ifdef DEBUG_1
  printf("daxpy called. size n = %d. alpha = %f, beta = %f \n", n, alpha, beta);
#endif
  for (i = 0; i < n; i++){
    result[i] = alpha*a[i]+beta*b[i];   
    //printf("daxpy result[i] = %f\n", result[i]);
  }
  return;
}