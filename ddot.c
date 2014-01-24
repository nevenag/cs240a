#include<stdio.h>
#include<assert.h>
#include "ddot.h"

// #define DEBUG_1

double ddot(double* a, double* b, int n){
  int i;  
  double result = 0;
  assert(n>0);  
#ifdef DEBUG_1
  printf("ddot called. size n = %d\n", n);
#endif
  for (i = 0; i < n; i++){
    result += a[i]*b[i];  
   // printf("ddot a[i] = %f * b[i] = %f \n", a[i], b[i]);
  }
#ifdef DEBUG_1
  printf("ddot result: %f\n", result);
#endif
  return result;
}