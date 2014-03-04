
#ifndef _CHROMA_FEAT
#define _CHROMA_FEAT

#include "rsrfft.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>

#define F                16000                              // original input rate
#define FS				 4000                               // downsampling rate
#define STARTNOTE           36                              // lowest pitch value
#define ENDNOTE             95                              // highest pitch value
#define NUMNOTES            60                              // num of pitches for detection
#define NUMCHROMAGRAM       12                              // dimension of chromagram
#define NUMONSETBIN         3                               // num of bins for onset detection
#define _2ROOT12		1.059463094f
#define FREQREF		7.943049790954413f       // the frequency of (midi = -0.5), which is the left boundary for C0
#define ZERO_PADDING_RATE	1.0f
#define PI              3.1415926f

class ChromaFeat {
public:
	int Chroma(const float* buffer);
	ChromaFeat(unsigned long lengthArg);
	~ChromaFeat();
    float maxima(float *x, int xsize);
    float *chromagram;                                      // 12 dimensional chromagram vector
    float *chroma;
    float *energysum;
    float *prechorma;
    const int chromagramdim = 12;
    double *gDataIn;
	
protected:
	SplitRadixFFT *fft;
	unsigned long length;
	float *hammingWin;
	float *powerOf2ROOT12;
	unsigned long FFT_Point;
	short FFT_Order;
    const float alfa = 0.4;
    const float alpha = 0.9;
    const float bita = 0.7;
};

#endif