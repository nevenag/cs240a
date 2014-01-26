/* UCSB CS240A, Winter Quarter 2014
 * Main and supporting functions for the Conjugate Gradient Solver on a 5-point stencil
 *
 * NAMES:
 *  Chris Horuk
 *  Nevena Golubovic
 * PERMS:
 *  4235677
 *  
 * DATE:
 *  1/28/14
 */
#include "mpi.h"
#include "hw2harness.h"
#include "cgsolve.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

double* load_vec( char* filename, int* k );
void save_vec( int k, double* x );

int main( int argc, char* argv[] ) {
	int writeOutX = 0;
	int n, k;
	int iterations = 1000;
	double norm;
	double* b;
	double time;
	double t1, t2;
	
	MPI_Init( &argc, &argv );
	
	// Read command line args.
	// 1st case runs model problem, 2nd Case allows you to specify your own b vector
	if ( argc == 3 ) {
		k = atoi( argv[1] );
		n = k*k;
		// each processor calls cs240_getB to build its own part of the b vector!
	} else if  ( !strcmp( argv[1], "-i" ) && argc == 4 ) {
		b = load_vec( argv[2], &k );
	} else {
		printf( "\nCGSOLVE Usage: \n\t"
			"Model Problem:\tmpirun -np [number_procs] cgsolve [k] [output_1=y_0=n]\n\t"
			"Custom Input:\tmpirun -np [number_procs] cgsolve -i [input_filename] [output_1=y_0=n]\n\n");
		exit(0);
	}
	writeOutX = atoi( argv[argc-1] ); // Write X to file if true, do not write if unspecified.
    double x[n];
    // Get number of processors and our processor number
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Sequential or parallel algorithm?
    if (size == 1)
    {
        // Sequential
    	// Start Timer
    	t1 = MPI_Wtime();
    	// CG Solve here!
        cgsolve_sequential(k, x, &norm, &iterations);
    	// End Timer
    	t2 = MPI_Wtime();
        // Write x to file if necessary
    	if ( writeOutX ) {
    		save_vec( k, x );
    	}
    	// Output
    	printf( "Problem size (k): %d\n",k);
    	printf( "Norm of the residual after %d iterations: %lf\n",iterations,norm);
    	printf( "Elapsed time during CGSOLVE: %lf\n", t1-t2);
        printf( "Result of cs240_verify: %d\n" , cs240_verify(x, k, t1-t2));
    }
    else
    {
        // Parallel
        int chunkSize = n/size;
        double tempX[chunkSize];
    	// Start Timer
    	t1 = MPI_Wtime();
    	// CG Solve here!
        if (rank == 0)
        {
            cgsolve_parallel(k, rank, size, x, &norm, &iterations);
        }
        else
        {
            cgsolve_parallel(k, rank, size, tempX, &norm, &iterations);
        }
    	// End Timer
    	t2 = MPI_Wtime();
        // Need to aggregate all the tempX vectors onto one processor now in the correct order
        int i, tag = 0;
        MPI_Status status;
        // 0 sends to 1, 1 adds its tempX at the end and sends to 2, and so on...
        for (i = 0; i < size; i++)
        {
            if (rank == i)
            {
                MPI_Send(x, n, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
            }
            else if (rank == i + 1)
            {
                MPI_Recv(x, n, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD, &status);
                // Need to place our tempX at the end of x now
                int xIndex = (i+1) * chunkSize, j;
                for (j = 0; j < chunkSize; j++)
                {
                    x[xIndex] = tempX[j];
                    xIndex++;
                }
            }
            // necessary?
            MPI_Barrier(MPI_COMM_WORLD);
        }
        // Last processor should have all of x now
        if (rank == size - 1)
        {
            // Write x to file if necessary
        	if ( writeOutX ) {
        		save_vec( k, x );
        	}
        	// Output
        	printf( "Problem size (k): %d\n",k);
        	printf( "Norm of the residual after %d iterations: %lf\n",iterations,norm);
        	printf( "Elapsed time during CGSOLVE: %lf\n", t1-t2);
            printf( "Result of cs240_verify: %d\n" , cs240_verify(x, k, t1-t2));
        }
    }
	
	MPI_Finalize();
	
	return 0;
}


/*
 * Supporting Functions
 *
 */

// Load Function
// NOTE: does not distribute data across processors
double* load_vec( char* filename, int* k ) {
	FILE* iFile = fopen(filename, "r");
	int nScan;
	int nTotal = 0;
	int n;
	
	if ( iFile == NULL ) {
		printf("Error reading file.\n");
		exit(0);
	}
	
	nScan = fscanf( iFile, "k=%d\n", k );
	if ( nScan != 1 ) {
		printf("Error reading dimensions.\n");
		exit(0);
	}
	
	n = (*k)*(*k);
	double* vec = (double *)malloc( n * sizeof(double) );
	
	do {
		nScan = fscanf( iFile, "%lf", &vec[nTotal++] );
	} while ( nScan >= 0 );
	
	if ( nTotal != n+1 ) {
		printf("Incorrect number of values scanned n=%d, nTotal=%d.\n",n,nTotal);
		exit(0);
	}
	
	return vec;
}

// Save a vector to a file.
void save_vec( int k, double* x ) { 
	FILE* oFile;
	int i;
	oFile = fopen("xApprox.txt","w");
	
	fprintf( oFile, "k=%d\n", k );
	
	for (i = 0; i < k*k; i++) { 
    	fprintf( oFile, "%lf\n", x[i]);
 	} 

	fclose( oFile );
}

