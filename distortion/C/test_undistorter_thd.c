#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <complex.h>

#include "undistorter.h"

#define NUMBER_OF_SAMPLES  10000000
#define TEST_FREQUENCY      1000.0f
#define SAMPLING_FREQUENCY 1000000.0f
#define NUMBER_OF_HARMONICS 6

float randn()
{
    int i;
    float retval = 0.0f;
    
    for(i = 0; i < 100; i++)
    {
        retval += ((float)rand())/RAND_MAX;
    }
    
    return (retval-50.0f)*0.34641016;
}

int main(int argc, char** argv)
{
    struct undistorter_ctx ctx;
    int i;
    int j;

    float complex dds_state;
    float complex dds_multiplier;

    float complex harmonics_in[  NUMBER_OF_HARMONICS];
    float complex harmonics_out[ NUMBER_OF_HARMONICS];
    
    float harmonic_power_in;
    float fundamental_power_in;
    
    float harmonic_power_out;
    float fundamental_power_out;
    
    float thd_in;
    float thd_out;
   
    undistorter_init(&ctx, 0.05, 2, 4, 1024, -1.5f, 1.5f, 1.0f);
    
    dds_state = 1.0f;
    dds_multiplier = cexpf(2.0f*I * M_PI * TEST_FREQUENCY/SAMPLING_FREQUENCY);
        
    for(i = 0; i < NUMBER_OF_HARMONICS; i++)
    {
        harmonics_in[i]  = 0.0f;
        harmonics_out[i] = 0.0f;
    }
    
    /*
     * We produce an input sinusoid x[n] = exp(j*2*pi*f/f_s*n).  The complex form
     * is convenient, because we can then calculate the power in each harmonic
     * by downconversion and time-averaging:
     *
     *      P_y[i] = < (y[n] (x[n]^n)*)^2 >
     *
     * where < > denotes a time average and z* the complex conjugate.
     */
    for(i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
        float noise = randn()*1e-2f;
        /* 1.13 was chosen to produce 10% THD. */
        float distorted_sinusoid = tanhf(1.13f*(crealf(dds_state)+ noise));
        float undistorted_sinusoid = undistorter_compensate_sample(&ctx, distorted_sinusoid);
        float complex carrier = conjf(dds_state);
        
        if(i > NUMBER_OF_SAMPLES/4)
        {
            for(j = 0; j < NUMBER_OF_HARMONICS; j++)
            {
                harmonics_in[j]  += carrier*distorted_sinusoid;
                harmonics_out[j] += carrier*undistorted_sinusoid;
                
                carrier *= conjf(dds_state);
            }
        }

        dds_state *= dds_multiplier;
    }
    
    fundamental_power_in   = cabsf(harmonics_in[0]  )*cabsf(harmonics_in[0]  );
    fundamental_power_out  = cabsf(harmonics_out[0] )*cabsf(harmonics_out[0] );
    
    harmonic_power_in = 0.0f;
    harmonic_power_out = 0.0f;
    
    for(i = 1; i < NUMBER_OF_HARMONICS; i++)
    {
        harmonic_power_in   += cabsf(harmonics_in[i]  )*cabsf(harmonics_in[i]  );
        harmonic_power_out  += cabsf(harmonics_out[i] )*cabsf(harmonics_out[i] );
    }
    
    thd_in = sqrtf((harmonic_power_in)  / fundamental_power_in)*100.0f;
    thd_out = sqrtf((harmonic_power_out) / fundamental_power_out)*100.0f;
    
    if( thd_in/thd_out  >= sqrtf(10.0f))
    {
        printf("PASS: THDin = %f, THDout = %f\n", thd_in, thd_out);
    }
    else
    {
        printf("FAIL: THDin = %f, THDout = %f\n", thd_in, thd_out);
    }

    return 0;
}