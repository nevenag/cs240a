#include<string.h>
#include "matvec.h"
#include "mpi.h"

// #define DEBUG_MV

extern void print_vector(double *vector, int size);

void matvec(double *in, double *out, int k)
{
    int rowi, coli, n = k*k;
    // Loop through each row of output
    for (rowi = 0; rowi < k; rowi++)
    {
        // Loop through each column of output
        for (coli = 0; coli < k; coli++)
        {
            int i = rowi * k + coli;
            // Only 5 possible non-zero values, the middle one always exists
            out[i] = 4 * in[i];
            // Now figure out which of the other 4 exist
            if (rowi != 0)
                out[i] -= in[i-k];
            if (coli != 0)
                out[i] -= in[i-1];
            if (coli != k-1)
                out[i] -= in[i+1];
            if (rowi != k-1)
                out[i] -= in[i+k];
        }
    }
    return;
}


void matvec_parallel(double *in, double *out, int k, int rank, int size)
{
    int n = k*k;
    MPI_Status status;
    int tag = 0;
    int i;
    double message[k];
    int share = (int)n/size;
    // divide b to p processors they said it will be divisable!
    
#ifdef DEBUG_MV
    printf("\nmatvec_parallel: k %d rank %d size %d \n", k, rank, size);
    printf("in = \n");
    print_vector(in, share);
#endif
    
    
    /*top chunk*/
    if (rank == 0) 
    {
       double in_tmp[share+k];
       
       MPI_Recv(message, k, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &status);
#ifdef DEBUG_MV
       printf("\nProcess 0 received following from process 1:\n");
       print_vector(message, k);
#endif
       MPI_Send(in+(share-k), k, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
#ifdef DEBUG_MV
       printf("\nProcess 0 sent following to process 1:\n");
       print_vector(in+(share-k), k);
#endif
       
       memcpy(in_tmp, in, share*sizeof(double));
       memcpy(in_tmp+share, message, k*sizeof(double));
       
       for (i = 0; i < share; i++)
       {
	 int rowi = i / k;
	 int coli = i % k;
	 // Only 5 possible non-zero values, the middle one always exists
         out[i] = 4 * in_tmp[i];
         // Now figure out which of the other 4 exist
         if (rowi != 0)
           out[i] -= in_tmp[i-k];
         if (coli != 0)
           out[i] -= in_tmp[i-1];
         if (coli != k-1)
           out[i] -= in_tmp[i+1];
         if (rowi != k-1)
           out[i] -= in_tmp[i+k];
      }
    }
    /*bottom chunk*/
    else if (rank == size - 1)
    {
      double in_tmp[share+k];
      MPI_Send(in, k, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD);
#ifdef DEBUG_MV
      printf("\nProcess %d sent following to process %d:\n", rank, rank-1);
      print_vector(in, k);
#endif


      MPI_Recv(message, k, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD, &status);
#ifdef DEBUG_MV
      printf("\nProcess %d received following from process %d:\n", rank, rank-1);
      print_vector(message, k);
#endif      
      
      memcpy(in_tmp, message, k*sizeof(double));
      memcpy(in_tmp+k, in, share*sizeof(double));
      
      for (i = k; i < share+k; i++)
       {
	 int universali = (rank * share) + (i-k);
	 int rowi = universali / k;
	 int coli = universali % k;
	 // Only 5 possible non-zero values, the middle one always exists
         out[i-k] = 4 * in_tmp[i];
         // Now figure out which of the other 4 exist
         if (rowi != 0)
           out[i-k] -= in_tmp[i-k];
         if (coli != 0)
           out[i-k] -= in_tmp[i-1];
         if (coli != k-1)
           out[i-k] -= in_tmp[i+1];
         if (rowi != k-1)
           out[i-k] -= in_tmp[i+k];
      }
    }
    else
    {
      double message1[k];
      double in_tmp[share+2*k];
      if(rank % 2 == 1)
      {
	MPI_Send(in, k, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD);
	MPI_Recv(message, k, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD, &status);
	MPI_Send(in+(share-k), k, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
	MPI_Recv(message1, k, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD, &status);
      }
      else
      {
	MPI_Recv(message1, k, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD, &status);
	MPI_Send(in+(share-k), k, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
	MPI_Recv(message, k, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD, &status);
	MPI_Send(in, k, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD);
      }

      memcpy(in_tmp, message, k*sizeof(double));
      memcpy(in_tmp+k, in, share*sizeof(double));
      memcpy(in_tmp+k+share, message1, k*sizeof(double));
      
      for (i = k; i < share+k; i++)
      {
	int universali = (rank * share) + (i-k);
	int rowi = universali / k;
	int coli = universali % k;
	// Only 5 possible non-zero values, the middle one always exists
        out[i-k] = 4 * in_tmp[i];
        // Now figure out which of the other 4 exist
        if (rowi != 0)
          out[i-k] -= in_tmp[i-k];
        if (coli != 0)
          out[i-k] -= in_tmp[i-1];
        if (coli != k-1)
          out[i-k] -= in_tmp[i+1];
        if (rowi != k-1)
          out[i-k] -= in_tmp[i+k];
      }
    }
#ifdef DEBUG_MV    
    printf("out = \n");
    print_vector(out, share);
#endif
    return;
}