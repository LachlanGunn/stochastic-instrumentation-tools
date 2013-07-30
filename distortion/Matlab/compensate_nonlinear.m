% COMPENSATE_NONLINEAR  Estimate and compensate for static nonlinearity.
%
%   y = compensate_nonlinear(x, block_size, filter_cutoff)
%
% Synopsis:
%
%       t = 0:0.2e-5:1;
%       
%       % Make a ramp.
%       input = (t-0.5)*4 + 1e-4*randn(size(t));
%
%       % Distort the ramp with a logistic function.
%       distorted =  ( 1./(1+exp(-input)) - 0.5 )*4;
%
%       % Undo the distortion.
%       est_input = compensate_nonlinear(distorted , 40, 1e-3);
%
%       % Plot the results
%       figure();
%       plot(t, input, 'b');
%       hold on;
%       plot(t, distorted, 'r');
%       plot(t, est_input, 'Color', [1, 0.5, 0]);
%       hold off;
%       legend('Input', 'Distorted', 'Compensated');
%
% The compensate_nonlinear function undoes static distortion
% using an assumption of constant noise variance.
%
% Parameters:
%
%   x -
%     A vector containing the signal to compensate.
%
%   block_size -
%     The size of the blocks over which the estimated
%     derivative will be computed.
%
%   filter_cutoff -
%     The cutoff frequencies of the filters used to separate
%     signal and noise, where 1.0 corresponds to half the
%     sample rate.
%
% Return Value:
%
%   y -
%     A copy of the parameter x with its distortion undone.
%
% Reference page in Help browser
%    <a href="matlab:doc('compensate_nonlinear');">doc compensate_nonlinear</a>
function y = compensate_nonlinear(x, block_size, filter_cutoff)

    % First, use a pair of filters to separate signal from noise.
    [b_lpf, a_lpf] = butter(5, filter_cutoff, 'low' );
    [b_hpf, a_hpf] = butter(5, filter_cutoff, 'high');
    
    signal = filter(b_lpf, a_lpf, x);
    noise  = filter(b_hpf, a_hpf, x);
       
    % Next, compute an estimate of the inverse transfer function derivative 
    % for each block.
    
    % The number of blocks.
    blocks = floor(length(x)/block_size);
    
    tf_derivative = zeros(blocks,1);
    signal_level  = zeros(blocks,1);
    
    rejected_blocks = 0;
    
    last_max_diff = 0;
       
    for i = 0:(blocks-1)
        
        block_noise   = noise( (i*block_size+1):((i+1)*block_size) );
        
        % We remove outliers by discarding any blocks that increase
        % dramatically in magnitude relative to the previous one.
        if max(abs((block_noise))) < 2*last_max_diff
            
            % Compute the derivative of the predistorting transformation.
            % It will be inversely proportional to the local gain and so
            % inversely proportional to the scale parameter of the noise.
            %tf_derivative(i+1) = sqrt( (block_size-1.5)/sum(block_noise.^2));
            tf_derivative(i+1) = 1.0/std(block_noise);
            
            % We need also the operating point of the device, which we
            % compute as the mean signal amplitude, averaged over the
            % entire block.
            signal_level(i+1)  = mean( signal( (i*block_size+1):((i+1)*block_size)) );
        else
            tf_derivative(i+1) = NaN;
            signal_level(i+1) = NaN;
            
            rejected_blocks = rejected_blocks+1;
        end
        
        last_max_diff = max(abs(block_noise));

    end
   
    tf_derivative(isnan(tf_derivative)) = [];
    signal_level(isnan(signal_level)) = [];
       
    % Sort by signal level.
    [signal_level, signal_level_idx] = sort(signal_level);
    tf_derivative = tf_derivative(signal_level_idx);

    % Now integrate to find the inverse transfer function.
    est_tf = cumtrapz(signal_level, tf_derivative);
    
    % Now undistort the original signal.
    y = interp1(signal_level, est_tf, x);
    
    % Use a robust linear fit over the central region to find a
    % reasonable gain+offset.
    [x_for_fit, sorted_indices] = sort(x);
    y_for_fit = y(sorted_indices);
    fit_region_start = floor(length(x)*14/29);
    fit_region_end = ceil(length(y)*15/29);
    b = robustfit(x_for_fit(fit_region_start:fit_region_end), y_for_fit(fit_region_start:fit_region_end));
    
    % Use the computed gain and offset to make the corrected signal look
    % as much like the original as possible.
    y = (y - b(1))/b(2);
    
end