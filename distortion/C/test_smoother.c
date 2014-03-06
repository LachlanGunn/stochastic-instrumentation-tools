#include <stdio.h>

#include "smoother.h"

struct smoother_ctx ctx;

int main(int argc, char** argv)
{
    double x;
    int i;
    float e;
    float e_total = 0.0f;

    smoother_init(&ctx, -1.0f, 1.0f, 10000.0f);
    
    for(i = 0; i <= 2000; i++)
    {
        x = -1.0f + i*1e-3f;
        smoother_process_point(&ctx, x, x*x*10.0f);
    }
    
    for(i = 1; i <= 199; i++)
    {
        x = -1.0f + i*1e-2f;
        e = smoother_evaluate(&ctx, x) - x*x*10.0f;
        e_total += e*e;
        
        printf("%f -> %f\n", x, e);
    }
    
    e_total /= 199.0f;
    
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