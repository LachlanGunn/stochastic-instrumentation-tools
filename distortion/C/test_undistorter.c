#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "undistorter.h"

int main(int argc, char** argv)
{
    struct undistorter_ctx ctx;
    FILE* fh;
    FILE* fh_out;
    float* input_samples;
    long sample_count = 0;
    long sample_buffer_size = (1<<16);
    int i;
    clock_t start_time;
    clock_t end_time;
   
    undistorter_init(&ctx, 0.1, 2, 1);
    
    fh = fopen("C:\\tmp\\test.dat", "r");
    
    if(NULL == fh)
    {
        fprintf(stderr, "ERROR: Could not open input file.\n");
        return 1;
    }
    
    fh_out = fopen("C:\\tmp\\out.dat", "w");
    if(NULL == fh_out)
    {
        fprintf(stderr, "ERROR: Could not open output file.\n");
        return 1;
    }
    

    input_samples = (float*)malloc( sample_buffer_size*sizeof(float) );
    if(NULL == input_samples)
    {
        return 1;
    }

    while(!feof(fh))
    {       
        if(sample_buffer_size == sample_count)
        {
            sample_buffer_size *= 2;
            input_samples = (float*)realloc((void*)input_samples, sample_buffer_size*sizeof(float));
        }

        fscanf(fh, "%f\n", &input_samples[sample_count]);
        sample_count++;       
    }
    
    start_time = clock();
    for(i = 0; i < sample_count; i++)
    {
        undistorter_process_sample(&ctx, input_samples[i], &input_samples[i]);
    }
    end_time = clock();
    printf("Elapsed time: %f\n", ((float)(end_time-start_time))/CLOCKS_PER_SEC);
    
    for(i = 0; i < sample_count; i++)
    {
        fprintf(fh_out, "%f\n", input_samples[i]);
    }
    
    fclose(fh);

    return 0;
}