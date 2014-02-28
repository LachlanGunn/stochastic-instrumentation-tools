#include <math.h>
#include <stdio.h>

#include "undistorter.h"

#include "filter.h"
#include "smoother.h"

void undistorter_init(struct undistorter_ctx* ctx, float cutoff, float fs, int recompensate_every)
{
    filter_biquad_init(&(ctx->filter), cutoff, fs);
    smoother_init(&(ctx->smoother));
    smoother_create_integral(&(ctx->integral), &(ctx->smoother));
    ctx->sample_count = 0;
    
    ctx->signal_sum = 0.0f;
    ctx->noise_sum = 0.0f;
    ctx->noise_sum_squares = 0.0f;
    
    ctx->signal_min = 0.0f;
    ctx->signal_max = 5.0f;
    
    ctx->blocks_since_recompensation = recompensate_every;
    ctx->recompensate_every = recompensate_every;
}

char undistorter_process_sample(struct undistorter_ctx* ctx, float x, float* y)
{
    float new_signal, new_noise;
    
    /* First use a LPF to separate signal and noise. */
    new_signal = filter_biquad_process(&(ctx->filter), x);
    new_noise  = x - new_signal;
    
    /* Collect statistics. */
    ctx->signal_sum         += new_signal;
    ctx->noise_sum          += new_noise;
    ctx->noise_sum_squares  += new_noise*new_noise;
    
    ctx->sample_count++;
    
    if(ctx->sample_count == BLOCK_SIZE)
    {       
        float mean_signal = ctx->signal_sum / BLOCK_SIZE;
        float sum_noise_square = (ctx->noise_sum_squares - (ctx->noise_sum * ctx->noise_sum)/BLOCK_SIZE ); 
        float std_noise_reciprocol  = sqrt( (BLOCK_SIZE-1.0f)/sum_noise_square );
       
        if(!isnan(std_noise_reciprocol) && !isnan(mean_signal))
        {
            smoother_process_point(&(ctx->smoother), mean_signal, std_noise_reciprocol);
            
            if(1) /* ctx->blocks_since_recompensation >= ctx->recompensate_every)*/
            {
                smoother_create_integral(&(ctx->integral), &(ctx->smoother));
            
                ctx->offset = ctx->signal_min - smoother_evaluate_integral(&(ctx->integral), ctx->signal_min);
                ctx->scale  = (ctx->signal_max - ctx->signal_min) / (smoother_evaluate_integral(&(ctx->integral), ctx->signal_max) - ctx->offset);
                
                ctx->blocks_since_recompensation = 0;
            }
            
            ctx->blocks_since_recompensation++;
        }
            
        ctx->sample_count = 0;
        
        ctx->signal_sum = 0.0f;
        ctx->noise_sum = 0.0f;
        ctx->noise_sum_squares = 0.0f;
        
        
    }
    *y = ctx->scale*(smoother_evaluate_integral(&(ctx->integral), x)+ctx->offset);
        
    return 1;
}