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
            int i = rowi * k + coli;
            // Only 5 possible non-zero values, the middle one always exists
            double entrysum = 4 * input_vector[i];
            // Now figure out which of the other 4 exist
            if (rowi != 0)
                entrysum -= input_vector[i-k];
            if (coli != 0)
                entrysum -= input_vector[i-1];
            if (coli != k-1)
                entrysum -= input_vector[i+1];
            if (rowi != k-1)
                entrysum -= input_vector[i+k];
            
            output_vector[vectori] = entrysum;
        }
    }
    return output_vector;
}