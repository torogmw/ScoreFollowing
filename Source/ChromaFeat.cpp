
#include "ChromaFeat.h"


ChromaFeat::ChromaFeat(unsigned long lengthArg)
{
	unsigned long i;
	length = lengthArg;
    chromagram = new float[NUMCHROMAGRAM];
    prechorma = new float[NUMCHROMAGRAM];
    chroma = new float[NUMNOTES];
    energysum = new float[NUMNOTES];
    memset(prechorma, 20, NUMCHROMAGRAM*sizeof(float));
    
	// only calculate these slow math once
	hammingWin = new float[length];
	powerOf2ROOT12 = new float[NUMNOTES + 1];
	float hammingCoeff = (float) 2*PI / (length - 1);
	for (i=0; i<length; i++) {
		hammingWin[i] = (float) (0.54 - 0.46 * cos(hammingCoeff * i));
	}
	for (i=0; i <= NUMNOTES; i++)
	{
		powerOf2ROOT12[i] = (float) pow(_2ROOT12, (i+STARTNOTE));
	}
    
	
	// compute necessary FFT number
	FFT_Point = 1;
	FFT_Order = 0;
	unsigned long minLength4FFT = (unsigned long) ((1 + ZERO_PADDING_RATE) * length);
    
	while (FFT_Point < minLength4FFT)
	{
		FFT_Point = FFT_Point << 1;
		FFT_Order ++;
	}
	fft = new SplitRadixFFT(FFT_Order);
}

ChromaFeat::~ChromaFeat()
{
    delete []chromagram;
    delete []chroma;
    delete []energysum;
    delete []prechorma;
	delete []hammingWin;
	delete []powerOf2ROOT12;
	delete fft;
}


int ChromaFeat::Chroma(const float* buffer) {
    
    // check if the input arguments are legal
	if ( buffer == NULL || (buffer+length-1) == NULL) {
		printf("Error: illegal arguments.");
		return -1;
	}
	
	unsigned long i, j, k;
    
	if (FFT_Point < length)
	{
		printf("Error: FFT_Point is larger than frame length.");
		return -1;
	}
	
	// We don't do 'in-place' FFT so we copy it into a new array first
	// In the meantime, we multiply a hamming window
	
	float* X = new float[FFT_Point];
	for (i=0; i<length; i++)
		X[i] = (float) buffer[i] * hammingWin[i];
    
	for (i=length; i<FFT_Point; i++)
		X[i] = 0;
	
	fft->XForm(X);
	
	// we will use the formula f = (2^(1/12))^n * f_ref
	// to transform midi notes into corresponding frequencies
	// and further, k = f/delta_f to transform into FFT indices
	float* indexBoundary = new float[NUMNOTES + 1];
	float freqResolution = (float)FS / FFT_Point;
    
	// transformation from midi note to FFT index
	for (i=0; i<=NUMNOTES; i++)
		indexBoundary[i] = (float) (powerOf2ROOT12[i] * FREQREF / freqResolution);
	
	// We can safely calculate chroma vector now
	
    unsigned long left;
    unsigned long right;
	
    // the 'i' loop covers each note, where '0' indicates C, '6' indicates 'F#', etc.
    for (i=0; i<NUMNOTES; i++)
    {
        chroma[i] = 0;
        // so this is how we determine both sides of FFT index
        left = (unsigned long) ceil(indexBoundary[i]);
        right = (unsigned long) floor(indexBoundary[i + 1]);
        
//        float* FFT_sub = new float[right - left + 1];
//        for (k=left; k<=right; k++)
//            FFT_sub[k-left] = X[k];
//        
//        // use tonality feature, by taking the maxima instead of the mean value within one pitch
//        chroma[i] = maxima(FFT_sub, right - left +1);
//        delete FFT_sub;
        
        
        float FFT_sum = 0;
        for (k=left; k<=right; k++)
            FFT_sum += (float) (sqrt(X[k] * X[k] + X[FFT_Point-k] * X[FFT_Point-k]));
        chroma[i] = FFT_sum/(right - left);
        
//        if (prechorma[i]>1)
//            chroma[i] = alfa*prechorma[i] + (1-alfa)*chroma[i];
    }
    
    // calculate the energy sum of the fundamental freq, its double freq and its tripple freq
    for (int k=0; k<NUMNOTES; k++){
        energysum[k] = chroma[k];
        if((k+12)<NUMNOTES)
            energysum[k] += alpha * chroma[k+12];
        if((k+19)<NUMNOTES)
            energysum[k] += bita * chroma[k+19];
    }
    
    // calculate chromagram value by looping through each key, i.e., C0, C1, C2 ...
    for(int k=0; k<NUMCHROMAGRAM; k++){
        chromagram[k] = 0;
        for(int j=0; j<(NUMNOTES/NUMCHROMAGRAM); j++)
            chromagram[k] += chroma[k+NUMCHROMAGRAM*j];
    }
    
    
	// the 'i' loop covers each note, where '0' indicates C, '6' indicates 'F#', etc.
//	for (i=0; i<NUMCHROMAGRAM; i++)
//	{
//		chromagram[i] = 0;
//        for (j=0; j<NUMNOTES; j=j+NUMCHROMAGRAM){
//            // so this is how we determine both sides of FFT index
//            left = (unsigned long) ceil(indexBoundary[i + j]);
//            right = (unsigned long) floor(indexBoundary[i + j + 1]);
//        
////          float FFT_sum = 0;
////            
////			// the 'k' loop sums over all FFT values belonging to one key.
////			for (k=left; k<=right; k++)
////			{
////				FFT_sum += (float) (sqrt(X[k] * X[k] + X[FFT_Point-k] * X[FFT_Point-k]));
////			}
////            
////			chromagram[i] += FFT_sum/(right - left);
//            
//            float* FFT_sub = new float[right - left + 1];
//            for (k=left; k<=right; k++)
//                FFT_sub[k-left] = (float) (sqrt(X[k] * X[k] + X[FFT_Point-k] * X[FFT_Point-k]));
//        
//            // use tonality feature, by taking the maxima instead of the mean value within one pitch
//            float aa = maxima(FFT_sub, right - left +1);
//            chromagram[i] += aa;
//            pitchgram[i+j] = aa;
//            
//            delete FFT_sub;
//        }
//        if (prechorma[i]>1)
//            chromagram[i] = alfa*prechorma[i] + (1-alfa)*chromagram[i];
//	}
    
    for (int i=0; i<NUMCHROMAGRAM; i++)
        prechorma[i] = chromagram[i];
    
    
	// clean up
	delete []X;
	delete []indexBoundary;
	
	return 0;
}

float ChromaFeat::maxima(float *x, int xsize)
{
    float max = x[0];
    for (int k=1; k<xsize; k++)
        if (x[k] > max)
            max = x[k];
    return max;
}