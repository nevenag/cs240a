#include<stdio.h>
#include<assert.h>
#include "ddot.h"

double ddot(double* a, double* b, int n){
  int i;  
  double result = 0;
  assert(n>0);  
  
  printf("ddot called. size n = %d\n", n);
  for (i = 0; i < n; i++){
    result += a[i]*b[i];  
   // printf("ddot a[i] = %f * b[i] = %f \n", a[i], b[i]);
  }
  printf("ddot result: %f\n", result);
  return result;
}