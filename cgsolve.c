#include <math.h>
#include "cgsolve.h"
#include "hw2harness.h"
#include "matvec.h"

#define PROPORTIONALITY_CONSTANT 5

double* cgsolve(int k)
{
    int n = pow(k, 2), i = 0;
    double x[n];
    for (i; i<n; i++)
    {
        x[i] = 0;
    }
    double r[n];
    for (i=0; i<n; i++)
    {
        r[i] = cs240_getB(i, n);
    }
    double *d = r;
    int maxiters = PROPORTIONALITY_CONSTANT * k, iter_index = 0;
    while (iter_index < maxiters)
    {
        iter_index++;
        double *matvec_result = matvec(d, k);
        double temp_d = ddot(matvec_result, d_T);
        double temp_r = ddot(r, r_T);
        double alpha = temp_r / temp_d;
        
        x = daxpy(alpha, d, 1, x);
        double *r_new = daxpy(1, r, -alpha, matvec_result);
        double beta = ddot(r_new, r_new_T) / ddot(r, r_T);
        r = r_new;
        d = daxpy(1, r, beta, d);
    }
    
    return x;
}