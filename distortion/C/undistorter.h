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

    int stats_every;
    int samples_since_stats;
    
    int recompensate_every;
    int blocks_since_recompensation;
};

/**
 * Initialise the undistorter.
 *
 * @param   ctx                 The undistorter context to initialise.
 * @param   cutoff              The cutoff frequency for the signal/noise filters.
 * @param   fs                  The sampling rate of the system.
 * @param   stats_every         The number of blocks between each calculation of statistics.
 * @param   recompensate_every  The number of blocks between each recompensation.
 * @param   min                 The minimum value of the input range.
 * @param   max                 The maximum value of the input range.
 * @param   time_constant       The time constant for the distortion representation.
 */
extern void undistorter_init(struct undistorter_ctx* ctx, float cutoff, float fs, int stats_every, int recompensate_every, float min, float max, float time_constant);

/**
 * Update the model to take a new sample into account.
 *
 * @param   ctx     The undistorter context to use.
 * @param   x       The sample to process.
 */
extern void undistorter_process_sample(struct undistorter_ctx* ctx, float x);

/**
 * Compensate a sample and update the model.
 *
 * @param   ctx     The undistorter context to use.
 * @param   x       The sample to process.
 *
 * @return  The compensated value.
 */
extern float undistorter_compensate_sample(struct undistorter_ctx* ctx, float x);

#endif /* UNDISTORTER_H */
