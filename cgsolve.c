#include <math.h>
#include <string.h>
#include <stdio.h>
#include "mpi.h"
#include "cgsolve.h"
#include "hw2harness.h"
#include "matvec.h"
#include "daxpy.h"
#include "ddot.h"

#define PROPORTIONALITY_CONSTANT 5
// #define DEBUG_1
#define DEBUG_2

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
// Helper methods for cgsolve_parallel that implement the different routines
// (i.e. different modes of operation in the main loop)
void cgsolve_master_routine(int k, int vector_size, double normB, double *r);
void cgsolve_slave_routine();
/**
 * This method should only get called if size > 1, i.e. more than one processor is available.
 * Per the problem description, this method also assumes that k is evenly divisible by size.
**/
void cgsolve_parallel(int k, int rank, int size, double* result, double* norm, int* num_iter)
{
    int vector_size = (k*k)/size, i = 0, tag = 0;
    double r[vector_size];
    // r = b
    for (i = 0; i < vector_size; i++)
    {
        r[i] = cs240_getB(i + vector_size*rank, k*k);
    }
    // Assumes size = 2^x
    int log_p = log(size)/log(2);
    // This is the master, who will collect all the results together
    if (rank == 0)
    {
        double btb = ddot_distributed(r, r, vector_size, rank, size);
        // Now calculate the norm of b
        double normB = sqrt(btb);
        // We should be the last one out
        MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG_2
        printf("normB = %f\n", normB);
#endif
        cgsolve_master_routine(k, vector_size, normB, r);
    }
    else
    {
        // This return value doesnt actually mean anything useful
        double garbage = ddot_distributed(r, r, vector_size, rank, size);
#ifdef DEBUG_2
        printf("Process #%d waiting to synchronize...\n", rank);
#endif
        // Some processes break out of ddot_distributed earlier than others, need to synchronize here
        MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG_2
        printf("Process #%d synchronized.\n", rank);
#endif
        cgsolve_slave_routine();
    }
}
/**
 * The master routine for cgsolve_parallel. There is only one master.
 * The r vector comes in already initialized to be the chunk of b we are
 * responsible for.
**/
void cgsolve_master_routine(int k, int vector_size, double normB, double *r)
{
    // TODO: Get this to work
    return;
    
    int maxiters = PROPORTIONALITY_CONSTANT * k, iter_index = 0, i = 0;
    double relres = 1.0;
    // Distributed vectors have only n/p elements each
    double x[vector_size], r_new[vector_size], d[vector_size];
    // x = zeros(n, 1)
    for (i = 0; i < vector_size; i++)
    {
        x[i] = 0;
    }
    // Initialize rtr
    double rtr = ddot(r, r, vector_size);
    // d = r
    memcpy(d, r, vector_size*sizeof(double));
    while (relres > .00001 && iter_index < maxiters)
    {
        iter_index++;
        // A*d
        double matvec_result[vector_size];
        matvec(d, matvec_result, k);
        // alpha = r'*r / d'*A*d
        double alpha = rtr / ddot(matvec_result, d, vector_size);
        // x = x + alpha*d
        daxpy(alpha, d, 1, x, vector_size, x);
        // rnew = r - alpha*A*d
        daxpy(1, r, -alpha, matvec_result, vector_size, r_new);
        // r' * r
        double rtrold = rtr;
        // r_new' * r_new
        rtr = ddot(r_new, r_new, vector_size);
        // Update residual
        relres = sqrt(rtr) / normB;
        // beta = rnew'*rnew / r'*r
        double beta = rtr / rtrold;
        // r = r_new
        memcpy (r, r_new, vector_size*sizeof(double));
        // d = r + beta*d
        daxpy(1, r, beta, d, vector_size, d);
    }
}
/**
 * The slave routine for cgsolve_parallel. Everyone is a slave besides the master.
**/
void cgsolve_slave_routine()
{
    // TODO: Implement this.
}