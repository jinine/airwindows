/* ========================================
 *  LRConvolve - LRConvolve.h
 *  Copyright (c) airwindows, Airwindows uses the MIT license
 * ======================================== */

#ifndef __LRConvolve_H
#include "LRConvolve.h"
#endif

void LRConvolve::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) 
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];
	    
    while (--sampleFrames >= 0)
    {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		//blame Jannik Asfaig (BoyXx76) for this (and me) :D
		double out = 0.0;		
		if (inputSampleL > 0.0 && inputSampleR > 0.0) out = sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		if (inputSampleL < 0.0 && inputSampleR > 0.0) out = -sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		if (inputSampleL > 0.0 && inputSampleR < 0.0) out = -sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		if (inputSampleL < 0.0 && inputSampleR < 0.0) out = sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		inputSampleL = inputSampleR = out;
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
    }
}

void LRConvolve::processDoubleReplacing(double **inputs, double **outputs, VstInt32 sampleFrames) 
{
    double* in1  =  inputs[0];
    double* in2  =  inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

    while (--sampleFrames >= 0)
    {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		//blame Jannik Asfaig (BoyXx76) for this (and me) :D
		double out = 0.0;		
		if (inputSampleL > 0.0 && inputSampleR > 0.0) out = sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		if (inputSampleL < 0.0 && inputSampleR > 0.0) out = -sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		if (inputSampleL > 0.0 && inputSampleR < 0.0) out = -sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		if (inputSampleL < 0.0 && inputSampleR < 0.0) out = sqrt(fabs(inputSampleL)*fabs(inputSampleR));
		inputSampleL = inputSampleR = out;
		
		//begin 64 bit stereo floating point dither
		//int expon; frexp((double)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		//inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
		//frexp((double)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		//inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
		//end 64 bit stereo floating point dither
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
    }
}
