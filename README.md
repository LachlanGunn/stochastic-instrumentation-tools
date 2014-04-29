Stochastic Instrumentation Tools
================================

<em>Lachlan J. Gunn, Andrew Allison, Derek Abbott</em>

<em>School of Electrical and Electronic Engineering, The University of Adelaide</em>

The Stochastic Instrumentation Tools libraries use stochastic signal
processing to provide enhanced measurement capabilities.
They have been developed by Lachlan J. Gunn as part of his doctoral study
under Prof. Derek Abbott and Dr. Andrew Allison at the University of Adelaide.

## Compensation of Distortion ##

We have developed a technique for the compensation of distortion without
knowledge of the input or the form of the distorting transfer function.
This is done by stretching regions of the signal so as to counteract
variations in small-signal gain across the output range.
The small-signal gain is measured by estimating the noise floor of the
measured signal.

We have developed a C implementation of the concept suitable for
embedded use, and which has been tested on the STM32F407 microcontroller.

We have developed proof-of-concept implementations for both Matlab and
Labview.  These can be found in the distortion/ directory.	

For more details, please visit

	https://www.eleceng.adelaide.edu.au/personal/dabbott/wiki/index.php/Compensation_of_distortion

See also:

	L. J. Gunn, A. Allison, and D. Abbott, "Identification of static distortion by noise measurement," Electronics Letters, Vol. 49, No. 21, pp. 1321–1323, 2013, doi:10.1049/el.2013.2547
