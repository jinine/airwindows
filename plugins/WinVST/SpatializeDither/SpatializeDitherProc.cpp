/* ========================================
 *  SpatializeDither - SpatializeDither.h
 *  Copyright (c) 2016 airwindows, All rights reserved
 * ======================================== */

#ifndef __SpatializeDither_H
#include "SpatializeDither.h"
#endif

void SpatializeDither::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) 
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

	long double inputSampleL;
	long double inputSampleR;
	
	double contingentRnd;
	double absSample;
	double contingent;
	double randyConstant = 1.61803398874989484820458683436563811772030917980576;
	double omegaConstant = 0.56714329040978387299996866221035554975381578718651;
	double expConstant = 0.06598803584531253707679018759684642493857704825279;
	
	    
    while (--sampleFrames >= 0)
    {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (inputSampleL<1.2e-38 && -inputSampleL<1.2e-38) {
			static int noisesource = 0;
			//this declares a variable before anything else is compiled. It won't keep assigning
			//it to 0 for every sample, it's as if the declaration doesn't exist in this context,
			//but it lets me add this denormalization fix in a single place rather than updating
			//it in three different locations. The variable isn't thread-safe but this is only
			//a random seed and we can share it with whatever.
			noisesource = noisesource % 1700021; noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003; residue *= residue;
			residue = residue % 17011; residue *= residue;
			residue = residue % 1709; residue *= residue;
			residue = residue % 173; residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleL = applyresidue;
		}
		if (inputSampleR<1.2e-38 && -inputSampleR<1.2e-38) {
			static int noisesource = 0;
			noisesource = noisesource % 1700021; noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003; residue *= residue;
			residue = residue % 17011; residue *= residue;
			residue = residue % 1709; residue *= residue;
			residue = residue % 173; residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleR = applyresidue;
			//this denormalization routine produces a white noise at -300 dB which the noise
			//shaping will interact with to produce a bipolar output, but the noise is actually
			//all positive. That should stop any variables from going denormal, and the routine
			//only kicks in if digital black is input. As a final touch, if you save to 24-bit
			//the silence will return to being digital black again.
		}

		inputSampleL *= 8388608.0;
		inputSampleR *= 8388608.0;
		//0-1 is now one bit, now we dither
		
		if (inputSampleL > 0) inputSampleL += 0.383;
		if (inputSampleL < 0) inputSampleL -= 0.383;
		if (inputSampleR > 0) inputSampleR += 0.383;
		if (inputSampleR < 0) inputSampleR -= 0.383;
		//adjusting to permit more information drug outta the noisefloor
		
		contingentRnd = (((rand()/(double)RAND_MAX)+(rand()/(double)RAND_MAX))-1.0) * randyConstant; //produce TPDF dist, scale
        contingentRnd -= contingentErrL*omegaConstant; //include err
		absSample = fabs(inputSampleL);
		contingentErrL = absSample - floor(absSample); //get next err
		contingent = contingentErrL * 2.0; //scale of quantization levels
		if (contingent > 1.0) contingent = ((-contingent+2.0)*omegaConstant) + expConstant;
		else contingent = (contingent * omegaConstant) + expConstant;
		//zero is next to a quantization level, one is exactly between them
		if (flip) contingentRnd = (contingentRnd * (1.0-contingent)) + contingent + 0.5;
		else contingentRnd = (contingentRnd * (1.0-contingent)) - contingent + 0.5;
		inputSampleL += (contingentRnd * contingent);
		//Contingent Dither
		inputSampleL = floor(inputSampleL);
		
		contingentRnd = (((rand()/(double)RAND_MAX)+(rand()/(double)RAND_MAX))-1.0) * randyConstant; //produce TPDF dist, scale
        contingentRnd -= contingentErrR*omegaConstant; //include err
		absSample = fabs(inputSampleR);
		contingentErrR = absSample - floor(absSample); //get next err
		contingent = contingentErrR * 2.0; //scale of quantization levels
		if (contingent > 1.0) contingent = ((-contingent+2.0)*omegaConstant) + expConstant;
		else contingent = (contingent * omegaConstant) + expConstant;
		//zero is next to a quantization level, one is exactly between them
		if (flip) contingentRnd = (contingentRnd * (1.0-contingent)) + contingent + 0.5;
		else contingentRnd = (contingentRnd * (1.0-contingent)) - contingent + 0.5;
		inputSampleR += (contingentRnd * contingent);
		//Contingent Dither
		inputSampleR = floor(inputSampleR);
		
		//note: this does not dither for values exactly the same as 16 bit values-
		//which forces the dither to gate at 0.0. It goes to digital black,
		//and does a teeny parallel-compression thing when almost at digital black.
		flip = !flip;
		
		
		inputSampleL /= 8388608.0;
		inputSampleR /= 8388608.0;
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		*in1++;
		*in2++;
		*out1++;
		*out2++;
    }
}

void SpatializeDither::processDoubleReplacing(double **inputs, double **outputs, VstInt32 sampleFrames) 
{
    double* in1  =  inputs[0];
    double* in2  =  inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

	long double inputSampleL;
	long double inputSampleR;
	
	double contingentRnd;
	double absSample;
	double contingent;
	double randyConstant = 1.61803398874989484820458683436563811772030917980576;
	double omegaConstant = 0.56714329040978387299996866221035554975381578718651;
	double expConstant = 0.06598803584531253707679018759684642493857704825279;
	

    while (--sampleFrames >= 0)
    {
		inputSampleL = *in1;
		inputSampleR = *in2;
		if (inputSampleL<1.2e-38 && -inputSampleL<1.2e-38) {
			static int noisesource = 0;
			//this declares a variable before anything else is compiled. It won't keep assigning
			//it to 0 for every sample, it's as if the declaration doesn't exist in this context,
			//but it lets me add this denormalization fix in a single place rather than updating
			//it in three different locations. The variable isn't thread-safe but this is only
			//a random seed and we can share it with whatever.
			noisesource = noisesource % 1700021; noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003; residue *= residue;
			residue = residue % 17011; residue *= residue;
			residue = residue % 1709; residue *= residue;
			residue = residue % 173; residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleL = applyresidue;
		}
		if (inputSampleR<1.2e-38 && -inputSampleR<1.2e-38) {
			static int noisesource = 0;
			noisesource = noisesource % 1700021; noisesource++;
			int residue = noisesource * noisesource;
			residue = residue % 170003; residue *= residue;
			residue = residue % 17011; residue *= residue;
			residue = residue % 1709; residue *= residue;
			residue = residue % 173; residue *= residue;
			residue = residue % 17;
			double applyresidue = residue;
			applyresidue *= 0.00000001;
			applyresidue *= 0.00000001;
			inputSampleR = applyresidue;
			//this denormalization routine produces a white noise at -300 dB which the noise
			//shaping will interact with to produce a bipolar output, but the noise is actually
			//all positive. That should stop any variables from going denormal, and the routine
			//only kicks in if digital black is input. As a final touch, if you save to 24-bit
			//the silence will return to being digital black again.
		}
		
		inputSampleL *= 8388608.0;
		inputSampleR *= 8388608.0;
		//0-1 is now one bit, now we dither
		
		if (inputSampleL > 0) inputSampleL += 0.383;
		if (inputSampleL < 0) inputSampleL -= 0.383;
		if (inputSampleR > 0) inputSampleR += 0.383;
		if (inputSampleR < 0) inputSampleR -= 0.383;
		//adjusting to permit more information drug outta the noisefloor
		
		contingentRnd = (((rand()/(double)RAND_MAX)+(rand()/(double)RAND_MAX))-1.0) * randyConstant; //produce TPDF dist, scale
        contingentRnd -= contingentErrL*omegaConstant; //include err
		absSample = fabs(inputSampleL);
		contingentErrL = absSample - floor(absSample); //get next err
		contingent = contingentErrL * 2.0; //scale of quantization levels
		if (contingent > 1.0) contingent = ((-contingent+2.0)*omegaConstant) + expConstant;
		else contingent = (contingent * omegaConstant) + expConstant;
		//zero is next to a quantization level, one is exactly between them
		if (flip) contingentRnd = (contingentRnd * (1.0-contingent)) + contingent + 0.5;
		else contingentRnd = (contingentRnd * (1.0-contingent)) - contingent + 0.5;
		inputSampleL += (contingentRnd * contingent);
		//Contingent Dither
		inputSampleL = floor(inputSampleL);
		
		contingentRnd = (((rand()/(double)RAND_MAX)+(rand()/(double)RAND_MAX))-1.0) * randyConstant; //produce TPDF dist, scale
        contingentRnd -= contingentErrR*omegaConstant; //include err
		absSample = fabs(inputSampleR);
		contingentErrR = absSample - floor(absSample); //get next err
		contingent = contingentErrR * 2.0; //scale of quantization levels
		if (contingent > 1.0) contingent = ((-contingent+2.0)*omegaConstant) + expConstant;
		else contingent = (contingent * omegaConstant) + expConstant;
		//zero is next to a quantization level, one is exactly between them
		if (flip) contingentRnd = (contingentRnd * (1.0-contingent)) + contingent + 0.5;
		else contingentRnd = (contingentRnd * (1.0-contingent)) - contingent + 0.5;
		inputSampleR += (contingentRnd * contingent);
		//Contingent Dither
		inputSampleR = floor(inputSampleR);
		
		//note: this does not dither for values exactly the same as 16 bit values-
		//which forces the dither to gate at 0.0. It goes to digital black,
		//and does a teeny parallel-compression thing when almost at digital black.
		flip = !flip;
		
		
		inputSampleL /= 8388608.0;
		inputSampleR /= 8388608.0;
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		*in1++;
		*in2++;
		*out1++;
		*out2++;
    }
}