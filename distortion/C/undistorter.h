#ifndef __UNDISTORTER_H
#define __UNDISTORTER_H

#include "filter.h"
#include "smoother.h"

#define BLOCK_SIZE 4

struct undistorter_ctx
{
    struct smoother_ctx             smoother;
    struct smoother_integrated_ctx  integral;
    
    struct filter_biquad_ctx        filter;
    
    int    sample_count;

    float  signal_sum;
    float   noise_sum;
    float   noise_sum_squares;
    
    float signal_min;
    float signal_max;
    
    float offset;
    float scale;
    
    int recompensate_every;
    int blocks_since_recompensation;
};

/**
 * Initialise the undistorter.
 *
 * @param   ctx                 The undistorter context to initialise.
 * @param   cutoff              The cutoff frequency for the signal/noise filters.
 * @param   fs                  The sampling rate of the system.
 * @param   recompensate_every  The number of blocks between each recompensation.
 * @param   min                 The minimum value of the input range.
 * @param   max                 The maximum value of the input range.
 * @param   time_constant       The time constant for the distortion representation.
 */
extern void undistorter_init(struct undistorter_ctx* ctx, float cutoff, float fs, int recompensate_every, float min, float max, float time_constant);

/**
 * Process a sample.
 *
 * @param   ctx     The undistorter context to use.
 * @param   x       The sample to process.
 * @param   y       A pointer to the output sample buffer;
 *
 * @return One if *y contains a new sample, zero otherwise.
 */
extern char undistorter_process_sample(struct undistorter_ctx* ctx, float x, float* y);

#endif /* UNDISTORTER_H */
