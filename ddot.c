#include<stdio.h>
#include<assert.h>
#include "ddot.h"

double ddot(double* a, double* b, int n){
  int i = 0;  
  double result = 0;
  assert(n>0);  
  for (i; i < n; i++){
    result += a[i]*b[i];   
  }
  return result;
}