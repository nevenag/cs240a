#include "matvec.h"

double *matvec(double *input_vector, double *output_vector, int k)
{
    int rowi, coli, n = k*k;
    // Loop through each row of output
    for (rowi = 0; rowi < k; rowi++)
    {
        // Loop through each column of output
        for (coli = 0; coli < k; coli++)
        {
            int vectori = rowi * k + coli;
            // Only 5 possible non-zero values, the middle one always exists
            int entrysum = 4 * input_vector[vectori];
            // Now figure out which of the other 4 exist
            int leftmost = vectori-k,
            leftinner = vectori-1,
            rightinner = vectori+1,
            rightmost = vectori+k;
            
            if (leftmost >= 0)
                entrysum -= input_vector[leftmost];
            if (leftinner >= 0)
                entrysum -= input_vector[leftinner];
            if (rightinner < n)
                entrysum += input_vector[rightinner];
            if (rightmost < n)
                entrysum += input_vector[rightmost];
            
            output_vector[vectori] = entrysum;
        }
    }
    return output_vector;
}