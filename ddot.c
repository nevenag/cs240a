#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "mpi.h"
#include "ddot.h"

// #define DEBUG_1
#define DEBUG_2

double ddot(double* a, double* b, int n){
  int i;  
  double result = 0;
  assert(n>0);  
#ifdef DEBUG_1
  printf("ddot called. size n = %d\n", n);
#endif
  for (i = 0; i < n; i++){
    result += a[i]*b[i];  
   // printf("ddot a[i] = %f * b[i] = %f \n", a[i], b[i]);
  }
#ifdef DEBUG_1
  printf("ddot result: %f\n", result);
#endif
  return result;
}
/**
 * Slightly different semantics than the sequential ddot.
 * Only the master process gets the actual value of the ddot returned to it,
 * the other processes just get their chunks returned back to them.
**/
double ddot_distributed(double* a, double* b, int n, int rank, int size)
{
    MPI_Status status;
    // Assumes size = 2^x
    int log_p = log(size)/log(2), i = 0, tag = 0;
    // Calculate our ddot chunk
    double ddot_chunk = ddot(a, b, n);
    // Need to aggregate all the ddot_chunk results
    if (rank == 0)
    {
        // This is the master, who will collect all the results together
        for (i = 0; i < log_p; i++)
        {
            int src_rank = pow(2, i);
            double message;
            MPI_Recv(&message, 1, MPI_DOUBLE, src_rank, tag, MPI_COMM_WORLD, &status);
#ifdef DEBUG_2
            printf("Process 0 received %f from process %d\n", message, src_rank);
#endif
            ddot_chunk += message;
        }
    }
    else
    {
        // Need to get the ddot_chunks back to master in log_p time
        for (i = 0; i < log_p; i++)
        {
            int increment = pow(2, i), rank_to_use = rank / increment;
            double message;
            if (rank_to_use % 2 == 0)
            {
                MPI_Recv(&message, 1, MPI_DOUBLE, rank + increment, tag, MPI_COMM_WORLD, &status);
#ifdef DEBUG_2
                printf("Process %d received %f from process %d\n", rank, message, rank + increment);
#endif
                ddot_chunk += message;
            }
            else
            {
                MPI_Send(&ddot_chunk, 1, MPI_DOUBLE, rank - increment, tag, MPI_COMM_WORLD);
#ifdef DEBUG_2
                printf("Process %d sent %f to process %d\n", rank, ddot_chunk, rank - increment);
#endif
                break;
            }
        }
    }
    return ddot_chunk;
}
