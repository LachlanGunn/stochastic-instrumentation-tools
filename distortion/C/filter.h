#ifndef __FILTER_H
#define __FILTER_H

struct filter_biquad_ctx
{
    float delays[2];
    
    float a[3];
    float b[3];
};

/**
 * Construct a second-order Butterworth filter.
 *
 * @param   ctx     The biquad filter to initialise.
 * @param   cutoff  The cutoff frequency of the filter.
 * @param   fs      The sampling frequency of the input.
 */
extern void filter_biquad_init(struct filter_biquad_ctx* ctx, float cutoff, float fs);

/**
 * Filter a new sample through the biquad.
 *
 * @param   ctx The filter to be applied.
 * @param   x   The sample to be processed.
 * @return      The new output value of the filter.
 */
extern float filter_biquad_process(struct filter_biquad_ctx* ctx, float x);

#endif /* __FILTER_H */