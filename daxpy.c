#include<stdio.h>
#include<assert.h>
#include "daxpy.h"

void daxpy(double alpha, double* a, double beta, double* b, int n, double* result) {

  int i = 0;  
  assert(n>0);  
  for (i; i < n; i++){
    result[i] = alpha*a[i]+beta*b[i];   
  }
  return;
}