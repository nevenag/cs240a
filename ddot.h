#ifndef DDOT_H
#define DDOT_H

double ddot(double* a, double* b, int n);
double ddot_distributed(double* a, double* b, int n, int rank, int size);

#endif