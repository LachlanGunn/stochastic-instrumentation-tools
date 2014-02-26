#ifndef __SMOOTHER_H
#define __SMOOTHER_H

#include <stdlib.h>

struct smoother_ctx
{
    size_t N;
    float min_value;
    float max_value;
    
    /* FIXME: These should not be fixed, but we need to allocate
              them _somewhere_. */
    float values[32];
    float weights[32];
};

struct smoother_integrated_ctx
{
    size_t N;
    float min_value;
    float max_value;
    
    float values[33];
};

/**
 * Initialise the smoother.
 *
 * @param ctx   The smoother context to be initialised.
 */
extern void smoother_init(struct smoother_ctx* ctx);

/**
 * Process a point.
 *
 * @param ctx   The smoother into which the point is to be incorporated.
 * @param x     The x-value of the new point.
 * @param y     The y-value of the new point.
 */
extern void smoother_process_point(struct smoother_ctx* ctx, float x, float y);

/**
 * Evaluate the function.
 *
 * @param ctx   The smoother approximating the function.
 * @param x     The value at which to evaluate the function.
 *
 * @return The estimated function value.
 */
extern float smoother_evaluate(struct smoother_ctx* ctx, float x);

/**
 * Prepare to integrate of the smoothed function.
 *
 * This integrates all of the triangles so that a more exact integral can
 * be computed relative quickly.
 *
 * @param ctx_int   The integrated smoother context to set up.
 * @param ctx       The smoother context to integrate.
 */
extern void smoother_create_integral(struct smoother_integrated_ctx* ctx_int, struct smoother_ctx* ctx);
 
#endif /* __SMOOTHER_H */