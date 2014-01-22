#include <math.h>
#include <string.h>
#include "cgsolve.h"
#include "hw2harness.h"
#include "matvec.h"
#include "daxpy.h"
#include "ddot.h"

#define PROPORTIONALITY_CONSTANT 5

void cgsolve(int k, double* result, double* norm, int* num_iter)
{
    int n = pow(k, 2), i;
    double r_new[n];
    // x = zeros(n, 1)
    double x[n];
    for (i = 0; i<n; i++)
    {
        x[i] = 0;
    }
    // r = b
    double r[n];
    for (i=0; i<n; i++)
    {
        r[i] = cs240_getB(i, n);
    }
    // calculate the norm of b
    double normB = sqrt(ddot(r, r, n));
    // static max iterations
    int maxiters = PROPORTIONALITY_CONSTANT * k, iter_index = 0;
    // d = r
    double d[n];
    memcpy(d, r, n*sizeof(double));
    // Return the number of iters to caller
    *num_iter = maxiters;
    while (iter_index < maxiters)
    {
        iter_index++;
        // A*d
        double matvec_result[n];
        matvec(d, matvec_result, k);
        // d' * A*d
        double temp_d = ddot(matvec_result, d, n);
        // r' * r
        double temp_r = ddot(r, r, n);
        // alpha = r'*r / d'*A*d
        double alpha = temp_r / temp_d;
        // x = x + alpha*d
        daxpy(alpha, d, 1, x, n, x);
        // rnew = r - alpha*A*d
        daxpy(1, r, -alpha, matvec_result, n, r_new);
        // beta = rnew'*rnew / r'*r
        double beta = ddot(r_new, r_new, n) / ddot(r, r, n);
        // r = r_new
        memcpy (r, r_new, n*sizeof(double));
        // d = r + beta*d
        daxpy(1, r, beta, d, n, d);
    }
    // calculate norm of r
    double normR = sqrt(ddot(r, r, n));
    // relative residual norm = norm(b-A*x)/norm(b)
    *norm = normR/normB;
    // Store x into result
    memcpy(result, x, n*sizeof(double));
    return;
}