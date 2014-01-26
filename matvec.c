#include<string.h>
#include "matvec.h"
#include "mpi.h"

double *matvec(double *in, double *out, int k)
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
    return out;
}


double *matvec_parallel(double *in, double *out, int k, int rank, int size)
{
    int rowi, coli, n = k*k;
    MPI_Status status;
    int tag = 0;
    int i;
    int ib = 0;
    int ibm[1];
    // devide b to p processors they said it will be divisable!
    int share = (int)n/size;
    
    if (rank == 0) /*master*/
    {

      double message[3*share];
      int start_rowi;
      int end_rowi;
      // remove duplicates out of the block, if possible
      double outp[share];
   
	
      // each slave to take from start-row to end-row from start-col to end-col
      for(i = 0; i < size; i++){
	// iterator through b
	ib = i*share;
	// send share elements before, share in the middle and share after it
	ibm[0]=ib;
	memcpy(message, in+(share*(i-1)), 3*share*sizeof(double));
	MPI_Send(ibm, 1, MPI_INT, i, tag, MPI_COMM_WORLD);	
	MPI_Send(message, 3*share, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);

	// barrier and merge	
	//not sure if I should stich it this way:
	// TODO check this
	for (i = 0; i < size; i++){
	  MPI_Recv(&outp, share, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
	  memcpy(out+(i*share), outp, share*sizeof(double));
	}
      }
      
    }else{ /*slave*/
      // receive the first msg with iterator
      MPI_Recv(&ibm, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
      // recalculate rowi and coli
      rowi = *ibm / k;
      coli = *ibm % k;
      double message[3*share];
      
      MPI_Recv(&message, 3*share, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
      double outp[share];
      double inp[3*share]; // in parallel
      memcpy(inp, message, 3*share*sizeof(double));
      
      // start from already assigned values
      for (rowi; rowi < k; rowi++)
      {
        for (coli; coli < k; coli++)
        {
            int ibm_new = rowi * k + coli;
            // Only 5 possible non-zero values, the middle one always exists
            outp[ibm_new] = 4 * inp[ibm_new];
            // Now figure out which of the other 4 exist
            if (rowi != 0)
                outp[ibm_new] -= inp[ibm_new-k];
            if (coli != 0)
                outp[ibm_new] -= inp[ibm_new-1];
            if (coli != k-1)
                outp[ibm_new] -= inp[ibm_new+1];
            if (rowi != k-1)
                outp[ibm_new] -= inp[ibm_new+k];
        }
      }
      
        // send vec back to master
        
        MPI_Send(outp, share, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
    }        

    // merge all in master up!
    
    return out;
}