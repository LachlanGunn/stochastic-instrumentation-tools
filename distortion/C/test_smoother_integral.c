#include <stdio.h>

#include "smoother.h"

struct smoother_ctx ctx;
struct smoother_integrated_ctx ctx_i;

int main(int argc, char** argv)
{
    double x;
    int i;
    float e;
    float e_total = 0.0f;

    smoother_init(&ctx);
    
    for(i = 0; i <= 2000; i++)
    {
        x = -1.0f + i*1e-3f;
        smoother_process_point(&ctx, x, x*10.0f);
    }
    
    smoother_create_integral(&ctx_i, &ctx);
    
    for(i = 0; i < ctx_i.N; i++)
    {
        x = -1.0f + i*(ctx_i.max_value - ctx_i.min_value)/(ctx_i.N-1);
        e = ctx_i.values[i] - 5*(x*x-1);
        e_total += e*e;
    }
    
    e_total /= ctx_i.N;
    
    if(e_total < 1e-4)
    {
        printf("PASS: E[e^2] = %e < 1e-4\n", e_total);
    }
    else
    {
        printf("FAIL: E[e^2] = %e > 1e-4\n", e_total);
    }
    
    return 0;
}