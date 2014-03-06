#include "filter.h"
#include <math.h>
#include <stdio.h>

void filter_biquad_init(struct filter_biquad_ctx* ctx, float cutoff, float fs)
{
    float C;
    float a0;
    
    cutoff /= fs/2;
    fs = 2.0f;
    
    C = 0.5f/fs/tanf(3.14159265f*cutoff/(fs*fs));
    
    /* Computed using the bilinear transform of 1/(s^2 + sqrt(2) s + 1). */

    /* We have a0 != 0, so normalise everything around this. */
    a0 = (4.0f*fs*fs*C*C + C*2.828427f*fs + 1.0f);

    /* The numerator coefficients are independent of cutoff. */
    ctx->b[0] = 1.0f/a0;
    ctx->b[1] = 2.0f/a0;
    ctx->b[2] = 1.0f/a0;
    
    /* We aren't so lucky with the denominator. */
    ctx->a[0] = 1.0f;
    ctx->a[1] = (-8.0f*C*C*fs*fs                  + 2.0f)/a0;
    ctx->a[2] = ( 4.0f*C*C*fs*fs - C*2.828427f*fs + 1.0f)/a0;
    
    /* Finally, set the initial conditions. */
    ctx->delays[0] = 0.0f;
    ctx->delays[1] = 0.0f;

}

float filter_biquad_process(struct filter_biquad_ctx* ctx, float x)
{
    float y;
    
    y = ctx->b[0]*x + ctx->delays[1];
    
    ctx->delays[1] = ctx->delays[0] + ctx->b[1]*x - ctx->a[1]*y;
    ctx->delays[0] =                  ctx->b[2]*x - ctx->a[2]*y;
    
    return y;
}