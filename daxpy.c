#include<stdio.h>
#include<assert.h>
#include "daxpy.h"

void daxpy(double alpha, double* a, double beta, double* b, int n, double* result) {

  int i;  
  assert(n>0);  
  printf("daxpy called. size n = %d. alpha = %f, beta = %f \n", n, alpha, beta);
  for (i = 0; i < n; i++){
    result[i] = alpha*a[i]+beta*b[i];   
    //printf("daxpy result[i] = %f\n", result[i]);
  }
  return;
}