#include <math.h>
#include <string.h>
#include <stdio.h>
#include "cgsolve.h"
#include "hw2harness.h"
#include "matvec.h"
#include "daxpy.h"
#include "ddot.h"

#define PROPORTIONALITY_CONSTANT 5
#define DEBUG_1

void print_vector(double *vector, int size)
{
  int i;
  for (i = 0; i < size; i++)
  {
    printf("\t%f\n", vector[i]);
  }
  return;
}

void cgsolve_sequential(int k, double* result, double* norm, int* num_iter)
{
    int n = k*k, maxiters = PROPORTIONALITY_CONSTANT * k, iter_index = 0, i = 0;
    double x[n], r[n], r_new[n], d[n];
    // x = zeros(n, 1)
    for (i = 0; i<n; i++)
    {
        x[i] = 0;
    }
    // r = b
    for (i=0; i<n; i++)
    {
        r[i] = cs240_getB(i, n);
    }
    // calculate the norm of b and set relres to 1.0 (relres = norm(b-Ax)/norm(b) = norm(b)/norm(b), x=zeros(n, 1))
    double normB = sqrt(ddot(r, r, n)), relres = 1.0;
    // Initialize rtr
    double rtr = ddot(r, r, n);
    // d = r
    memcpy(d, r, n*sizeof(double));
    while (relres > .00001 && iter_index < maxiters)
    {
#ifdef DEBUG_1
        printf("\nIteration %d\n", iter_index);
#endif
        iter_index++;
        // A*d
        double matvec_result[n];
        matvec(d, matvec_result, k);
        // alpha = r'*r / d'*A*d
        double alpha = rtr / ddot(matvec_result, d, n);
        // x = x + alpha*d
        daxpy(alpha, d, 1, x, n, x);
        // rnew = r - alpha*A*d
        daxpy(1, r, -alpha, matvec_result, n, r_new);
        // r' * r
        double rtrold = rtr;
        // r_new' * r_new
        rtr = ddot(r_new, r_new, n);
        // Update residual
        relres = sqrt(rtr) / normB;
        // beta = rnew'*rnew / r'*r
        double beta = rtr / rtrold;
        // r = r_new
        memcpy (r, r_new, n*sizeof(double));
        // d = r + beta*d
        daxpy(1, r, beta, d, n, d);
#ifdef DEBUG_1
        printf("Ad = \n");
        print_vector(matvec_result, n);
        printf("alpha = %f\n", alpha);
        printf("x = \n");
        print_vector(x, n);
        printf("r = \n");
        print_vector(r, n);
        printf("rtrold = %f\n", rtrold);
        printf("rtr = %f\n", rtr);
        printf("beta = %f\n", beta);
        printf("d = \n");
        print_vector(d, n);
        printf("relres = %f\n", relres);
#endif
    }
    // Return the number of iters to caller
    *num_iter = iter_index;
    // relative residual norm = norm(b-A*x)/norm(b)
    *norm = relres;
    // Store x into result
    memcpy(result, x, n*sizeof(double));
    return;
}
/**
 * This method should only get called if size > 1, i.e. more than one processor is available.
 * Per the problem description, this method also assumes that k is evenly divisible by size.
**/
void cgsolve_parallel(int k, int rank, int size, double* result, double* norm, int* num_iter)
{
    int vector_size = (k*k)/size, maxiters = PROPORTIONALITY_CONSTANT * k, iter_index = 0, i = 0;
    // Distributed vectors have only n/p elements each
    double x[vector_size], r[vector_size], r_new[vector_size], d[vector_size];
    // x = zeros(n, 1)
    for (i = 0; i < vector_size; i++)
    {
        x[i] = 0;
    }
    // r = b
    for (i = 0; i < vector_size; i++)
    {
        r[i] = cs240_getB(i, n);
    }
    
}