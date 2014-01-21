#include <math.h>
#include "cgsolve.h"
#include "hw2harness.h"
#include "matvec.h"
#include "daxpy.h"
#include "ddot.h"

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
        double matvec_result[n];
        matvec(d, matvec_result, k);
	// n is the size of d and matvec_result
        double temp_d = ddot(matvec_result, d, n);
        double temp_r = ddot(r, r, n);
        double alpha = temp_r / temp_d;
        
        daxpy(alpha, d, 1, x, n, x);
        r_new = daxpy(1, r, -alpha, matvec_result, n, r_new);
        double beta = ddot(r_new, r_new) / ddot(r, r);
        r = r_new;
        daxpy(1, r, beta, d, n, d);
    }
    
    return x;
}