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

    smoother_init(&ctx, -1.0f, 1.0f);
    
    for(i = 0; i <= 20000; i++)
    {
        x = -1.0f + i*1e-4f;
        smoother_process_point(&ctx, x, x*10.0f);
    }
    
    smoother_create_integral(&ctx_i, &ctx);
    
    for(i = 1; i <= 199; i++)
    {
        x = -1.0f + i*1e-2f;
        e = smoother_evaluate_integral(&ctx_i, x) - 5*(x*x-1);
        e_total += e*e;
    }
    
    e_total /= 199;
    
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