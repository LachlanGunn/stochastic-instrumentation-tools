#include <math.h>
#include <stdio.h>

#include "undistorter.h"

#include "filter.h"
#include "smoother.h"

#ifdef USE_CMSIS
    #include "arm_math.h"
#endif

void undistorter_init(struct undistorter_ctx* ctx, float cutoff, float fs, int stats_every, int recompensate_every, float min, float max, float time_constant)
{

    filter_biquad_init(&(ctx->filter), cutoff, fs);

    smoother_init(&(ctx->smoother), min, max, time_constant*fs/BLOCK_SIZE);
    smoother_create_integral(&(ctx->integral), &(ctx->smoother));

    ctx->sample_count = 0;

    ctx->signal_sum = 0.0f;
    ctx->noise_sum = 0.0f;
    ctx->noise_sum_squares = 0.0f;
    
    ctx->signal_min = min;
    ctx->signal_max = max;
    
    ctx->offset = 0.0f;
    ctx->scale = -1.0f;
    
    ctx->samples_since_stats = stats_every;
    ctx->stats_every = stats_every;
    
    ctx->blocks_since_recompensation = recompensate_every;
    ctx->recompensate_every = recompensate_every;

}

void undistorter_process_sample(struct undistorter_ctx* ctx, float x)
{
    float new_signal, new_noise;
       
    /* First use a LPF to separate signal and noise. */
    new_signal = filter_biquad_process(&(ctx->filter), x);
    new_noise  = x - new_signal;
    
    if(ctx->samples_since_stats < ctx->stats_every)
    {
        ctx->samples_since_stats++;
    }
    else
    {
            
        /* Collect statistics. */
        ctx->signal_sum         += new_signal;
        ctx->noise_sum          += new_noise;
        ctx->noise_sum_squares  += new_noise*new_noise;
        
        ctx->sample_count++;
        
        if(ctx->sample_count == BLOCK_SIZE)
        {    
            float mean_signal = ctx->signal_sum / BLOCK_SIZE;
            float sum_noise_square = (ctx->noise_sum_squares - (ctx->noise_sum * ctx->noise_sum)/BLOCK_SIZE ); 
#ifdef USE_CMSIS
            float std_noise_reciprocol;
            arm_sqrt_f32( (BLOCK_SIZE-1.0f)/sum_noise_square, &std_noise_reciprocol );
#else
            float std_noise_reciprocol  = sqrtf( (BLOCK_SIZE-1.0f)/sum_noise_square );
#endif
           
            if(sum_noise_square > 0 && !isnan(std_noise_reciprocol) && !isnan(mean_signal))
            {
                smoother_process_point(&(ctx->smoother), mean_signal, std_noise_reciprocol);
                
                if( ctx->blocks_since_recompensation >= ctx->recompensate_every)
                {
                    float integral_min, integral_max;
                    float new_scale;
                    smoother_create_integral(&(ctx->integral), &(ctx->smoother));
                    
                    integral_min = smoother_evaluate_integral(&(ctx->integral), ctx->signal_min);
                    integral_max = smoother_evaluate_integral(&(ctx->integral), ctx->signal_max);

                    new_scale = 0.9f*(ctx->signal_max - ctx->signal_min) / (integral_max-integral_min);
                    if(new_scale < ctx->scale || ctx->scale < 0.0f)
                    {
                        ctx->scale  = new_scale;
                        ctx->offset = ctx->signal_min - integral_min + 0.05f*(integral_max - integral_min);
                    }

                    ctx->blocks_since_recompensation = 0;
                }
                
                ctx->blocks_since_recompensation++;
            }
                
            ctx->sample_count = 0;
            
            ctx->signal_sum = 0.0f;
            ctx->noise_sum = 0.0f;
            ctx->noise_sum_squares = 0.0f;
            
            ctx->samples_since_stats = 0;
        }
    }
}

float undistorter_compensate_sample(struct undistorter_ctx* ctx, float x)
{
    undistorter_process_sample(ctx, x);
    return smoother_evaluate_integral(&ctx->integral, x);
}