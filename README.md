Stochastic Instrumentation Tools
================================

<em>Lachlan J. Gunn, Andrew Allison, Derek Abbott
School of Electrical and Electronic Engineering
The University of Adelaide</em>

The Stochastic Instrumentation Tools libraries use stochastic signal
processing to provide enhanced measurement capabilities.
They have been developed by Lachlan J. Gunn as part of his doctoral study
under Prof. Derek Abbott and Dr. Andrew Allison at the University of Adelaide.

== Compensation of Distortion ==

We have developed a technique for the compensation of distortion without
knowledge of the input or the form of the distorting transfer function.
This is done by stretching regions of the signal so as to counteract
variations in small-signal gain across the output range.
The small-signal gain is measured by estimating the noise floor of the
measured signal.

We have developed proof-of-concept implementations for both Matlab and
Labview.  These can be found in the distortion/ directory.

For more details, please visit

	https://www.eleceng.adelaide.edu.au/personal/dabbott/wiki/index.php/Compensation_of_distortion