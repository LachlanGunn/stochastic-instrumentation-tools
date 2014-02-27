#include <stdlib.h>

#include "smoother.h"

/**
 * Convert a location in the input domain into an array index.
 *
 * @param ctx   The context for the smoother in question.
 * @param x     The point whose index is to be located.
 *
 * @return The smallest index whose region of interest contains the point.
 */
static float smoother_find_index(struct smoother_ctx* ctx, float x)
{   
    return (x - ctx->min_value)/(ctx->max_value - ctx->min_value)*(ctx->N-1.0f);
}

/**
 * Convert a location in the input domain into an array index.
 *
 * @param ctx_int   The context for the integral in question.
 * @param x         The point whose index is to be located.
 *
 * @return The smallest index whose region of interest contains the point.
 */
static float smoother_find_index_integral(struct smoother_integrated_ctx* ctx, float x)
{   
    return (x - ctx->min_value)/(ctx->max_value - ctx->min_value)*(ctx->N-1.0f);
}

void smoother_init(struct smoother_ctx* ctx)
{
    ctx->N = SMOOTHER_POINTS;
    ctx->min_value = -1.0f;
    ctx->max_value =  1.0f;
}

void smoother_process_point(struct smoother_ctx* ctx, float x, float y)
{
    float idx_f;
    
    int idx_integer;
    float idx_fractional;
    float weight_0;
    float weight_1;

    /* First find where in the signal range the point lies. */
    idx_f = smoother_find_index(ctx, x);

    /* Check for out-of-range. */
    if(idx_f < 0 || idx_f >= ctx->N - 1)
    {
        return;
    }
    
    /* Split the index into integer and fractional parts. */
    idx_integer = (int)idx_f;
    idx_fractional = idx_f - (float)idx_integer;
    
    /*
     * The integer part is the index of the reference before the point,
     * and the fractional part tells how far into the interval it is.
     * This is rather convenient, since the fractional part thus provides the
     * weight.
     */
    
    weight_0 = 1 - idx_fractional;
    weight_1 =     idx_fractional;
    
    /* Add to the weighted average. */
    ctx->weights[idx_integer] += weight_0;
    ctx->values[idx_integer] += weight_0*y;
    
    ctx->weights[idx_integer+1] += weight_1;
    ctx->values[idx_integer+1] += weight_1*y;
}

float smoother_evaluate(struct smoother_ctx* ctx, float x)
{
    float idx_f;
    
    int idx_integer;
    float idx_fractional;
    
    float weight_0;
    float weight_1;
    
    float value_0;
    float value_1;
    
    /* First find where in the signal range the point lies. */
    idx_f = smoother_find_index(ctx, x);
    
    /* Check for out-of-range. */
    if(idx_f < 0)
    {
        idx_f = 0.0f;
    }
    else if(idx_f > ctx->N)
    {
        idx_f = ctx->N;
    }
    
    idx_integer = (int)idx_f;
    idx_fractional = idx_f - (float)idx_integer;
    
    /*
     * The integer part is the index of the reference before the point,
     * and the fractional part tells how far into the interval it is.
     * This is rather convenient, since the fractional part thus provides the
     * weights.
     */
    weight_0 = 1 - idx_fractional;
    weight_1 =     idx_fractional;
    
    value_0  = ctx->values[idx_integer  ] / ctx->weights[idx_integer  ];
    value_1  = ctx->values[idx_integer+1] / ctx->weights[idx_integer+1];
    
    return weight_0*value_0 + weight_1*value_1;
}

void smoother_create_integral(struct smoother_integrated_ctx* ctx_int, struct smoother_ctx* ctx)
{
    int i;
    float max_0, max_1, width;
    
    ctx_int->N = ctx->N;
    ctx_int->min_value = ctx->min_value;
    ctx_int->max_value = ctx->max_value;
    
    /*
     * In this function we just set up the values from which to interpolate.
     * This means integrating up all of triangles, which is rather
     * straightforward---they each have area 0.5*w*h.
     */
    ctx_int->interp_c0[0] = 0.0f;
    
    for(i = 0; i < ctx->N; i++)
    {
        width = (ctx->max_value - ctx->min_value)/(ctx->N - 1);
        max_0 = ctx->values[i-1]/ctx->weights[i-1];
        max_1 = ctx->values[i  ]/ctx->weights[i  ];

        /* Zero is a special case. */
        if(i > 0)
        {
            ctx_int->interp_c0[i] = ctx_int->interp_c0[i-1] +
                0.5*width*(max_0 + max_1);
        }
        
        /*
         * Integrate max_1*( (x-x_0)/width ) + max_0*( 1 - (x-x_0)/width )
         * to get these coefficients, substituting x = width*u+x_0,
         * so as to rescale from 0 to 1.
         */
        ctx_int->interp_c1[i-1] = max_0*width;
        ctx_int->interp_c2[i-1] = 0.5f*(max_1-max_0)*width;
    }
}

float smoother_evaluate_integral(struct smoother_integrated_ctx* ctx_int, float x)
{
    float idx_f;
    
    int idx_integer;
    float idx_fractional;
    
    float weight_0;
    float weight_1;
    
    float value_0;
    float value_1;
    
    float integral_value;
    
    /* First find where in the signal range the point lies. */
    idx_f = smoother_find_index_integral(ctx_int, x);
    
    /* Check for out-of-range. */
    if(idx_f < 0.0f)
    {
        idx_integer = 0;
        idx_fractional = 0.0f;
    }
    else if(idx_f > ctx_int->N)
    {        
        idx_integer = ctx_int->N-1;
        idx_fractional = 1.0f;
    }
    else
    {
        idx_integer = (int)idx_f;
        idx_fractional = idx_f - (float)idx_integer;
    }
    
    /* Now evaluate the integral.  First the "whole" part. */
    integral_value = ctx_int->interp_c0[idx_integer];
    
    /*
     * We have computed coefficients for the quadratics
     * between each reference point in terms of the fractional
     * parts of the indices.  We evaluate the polynomial and add it on.
     */
     
    /* The linear term of the fractional part. */
    integral_value += ctx_int->interp_c1[idx_integer]*idx_fractional;
    
    /* Finally the quadratic. */
    integral_value += ctx_int->interp_c2[idx_integer]*idx_fractional*idx_fractional;
    
    return integral_value;
}