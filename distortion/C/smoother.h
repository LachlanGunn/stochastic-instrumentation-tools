#ifndef __SMOOTHER_H
#define __SMOOTHER_H

#include <stdlib.h>

#define SMOOTHER_POINTS 128

struct smoother_ctx
{
    size_t N;
    float min_value;
    float max_value;
    
    float decay_constant;
    
    /* FIXME: These should not be fixed, but we need to allocate
              them _somewhere_. */
    float values[SMOOTHER_POINTS];
    float weights[SMOOTHER_POINTS];
};

struct smoother_integrated_ctx
{
    size_t N;
    float min_value;
    float max_value;
    
    /* Coefficients for the interpolating quadratics. */
    float interp_c0[SMOOTHER_POINTS];
    float interp_c1[SMOOTHER_POINTS];
    float interp_c2[SMOOTHER_POINTS];
};

/**
 * Initialise the smoother.
 *
 * @param ctx               The smoother context to be initialised.
 * @param min               The minimum value to be considered.
 * @param max               The maximum value to be considered
 * @param time_constant     The time constant of the smoother.
 */
extern void smoother_init(struct smoother_ctx* ctx, float min, float max, float time_constant);

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
 
/**
 * Evaluate the indefinite integral.
 *
 * @param ctx_int   The integrated smoother to evaluate.
 * @param x         The value at which to evaluate the integral.
 *
 * @return The estimated indefinite integral.
 */
extern float smoother_evaluate_integral(struct smoother_integrated_ctx* ctx_int, float x);
 
#endif /* __SMOOTHER_H */