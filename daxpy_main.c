#include<stdio.h>
#include<assert.h>
#include "daxpy.h"


void main(int argc, char** argv){
  int i=0;
  double result[5];
  double a[5] = {1.0 ,2.2 ,3, 4, 5.3};
  double b[5] = {2.0, 4, 6, 8, 10.5};
  daxpy(3, a, 10, b, 5, result);
  
  for(i; i < 5; i++){
     printf("result[i] = %f \n", result[i]);  
  }
     
  return;
}
