#include<stdio.h>
#include<assert.h>
#include "ddot.h"


void main(int argc, char** argv){
  int i=0;
  double result;
  double a[5] = {1.0 ,2.2 ,3, 4, 5.3};
  double b[5] = {2.0, 4, 6, 8, 10.5};
  result = ddot(a, b, 5); 
  printf("result = %f \n", result);  
   
  return;
}
