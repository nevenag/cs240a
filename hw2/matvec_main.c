#include <stdio.h>
#include <stdlib.h>
#include "matvec.h"

void print_vector(double *vector, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        printf("%f\n", vector[i]);
    }
}

int main(int argc, char *argv[])
{
    int k = atoi(argv[1]);
    int n = k*k;
    double input_vector[n];
    int i;
    for (i = 0; i < n; i++)
    {
        input_vector[i] = 1;
    }
    printf("Input Vector:\n");
    print_vector(input_vector, n);

    double result[n];
    matvec(input_vector, result, k);
    printf("Output Vector:\n");
    print_vector(result, n);
    return 0;
}