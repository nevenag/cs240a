#ifndef MATVEC_H
#define MATVEC_H

void matvec(double *input_vector, double *output_vector, int k);

void matvec_parallel(double *input_vector, double *output_vector, int k, int rank, int size);
#endif