#include <stdio.h>
#include <math.h>

#include "filter.h"

#define N 32768

int main(int argc, char** argv)
{
    long i ;
    float energy = 0.0f;
    struct filter_biquad_ctx ctx;
    float error;
    
    ctx.a[0] =  1.0000f;
    ctx.a[1] = -1.5610f;
    ctx.a[2] =  0.6414f;
    
    ctx.b[0] = 0.02008f;
    ctx.b[1] = 0.04017f;
    ctx.b[2] = 0.02008f;
    
    ctx.delays[0] = 0.0f;
    ctx.delays[1] = 0.0f;
    
    filter_biquad_init(&ctx, 0.2f, 2.0f);
    
    printf("a = [ %f %f %f ], b = [ %f %f %f]\n",
            ctx.a[0], ctx.a[1], ctx.a[2],
            ctx.b[0], ctx.b[1], ctx.b[2]);
        
    /* |H(0.1)|^2 = 0.05344 */
    for(i = 0; i < N; i++)
    {
        float x = cos(i*2*3.141592653589796f*0.1);
        float y = filter_biquad_process(&ctx, x);
        
        /* Skip the first few samples while it settles. */
        if(i >= 100)
        {
            energy += y*y;
        }
    }
    
    /* We use an arbitrary threshold of 0.1% error. */
    error = fabs(2.0f*energy/(float)(N-100) - 0.05344);
    if( error/0.053443215f > 1e-4 )
    {
        printf("FAIL: e = %f%%\n", error/0.0005344);
    }
    else
    {
        printf("PASS: e = %f%%\n", error/0.0005344);
    }
    
    return 0;
}