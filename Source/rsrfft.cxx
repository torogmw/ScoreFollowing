/* ----------------------------------------------------------- */
/*    File: rsrfft.cxx  Real-valued, split-radix FFT           */
/*    Date: 06/10/96                                           */
/*    Author: Kai Huebener                                     */
/* ----------------------------------------------------------- */

//#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rsrfft.h"

//using namespace std;

#ifdef PI 
  #undef PI                                      // StH: undefine PI
#endif
static const float PI = 3.14159265358979323846F;
static const float TWO_PI = 6.28318530717958647692F;
static const float SQRT1_2 = 0.70710678118654752440F;

SplitRadixFFT::SplitRadixFFT(const int order)
   : M(order),N(2<<(order-1)),NBIT(2<<(order/2-1))
{
   const int M2 = (M & 0x01) ? M/2+1 : M/2;
   float x;
   int IMAX,LBSS;

   SINE = new float[M-1];
   COSINE = new float[M-1];
   ITAB = new int[2<<(M2-1)] - 1;

   for (IMAX=8,LBSS=0; LBSS<M-1; LBSS++) {
      IMAX *= 2;
      x = TWO_PI / IMAX;
       SINE[LBSS] = (float)sin(x);
      COSINE[LBSS] = (float)cos(x);
   }
   
   ITAB[1] = 0;
   ITAB[2] = 1;

   for (IMAX=1,LBSS=2; LBSS<=M2; LBSS++) {
      IMAX *= 2;
      for (int I=1; I<=IMAX; I++) {
	 ITAB[I] *= 2;
	 ITAB[I+IMAX] = ITAB[I] + 1;
      }
   }
}

SplitRadixFFT::~SplitRadixFFT()
{
   delete [] (ITAB+1);
   delete [] SINE;
   delete [] COSINE;
   ITAB = NULL;
}

void SplitRadixFFT::XForm(float *X)
{
   int I,I0,I1,I2,IS,ID;
   int J,J0,JN,K,L,N2,N4,N8;
   float CC1,CD1,CD3,SD1,SD3,SS1,CC3,SS3,*X1,*X2,*X3,*X4,XT;
   float T1,T2,T3,T4,T5;
   float *sine,*cosine;

/* ----Digit reverse counter--------------------------------------------- */
   for (X--,K=2; K<=NBIT; K++) {
      J0 = NBIT * ITAB[K] + 1;
      I = K;
      J = J0;
      for (L=2; L<=ITAB[K]+1; L++) {
	 XT = X[I];
	 X[I] = X[J];
	 X[J] = XT;
	 I += NBIT;
	 J = J0 + ITAB[L];
      }
   }
	 
/* ----Length two butterflies--------------------------------------------- */
   IS = 1;
   ID = 4;
   do {
      for (I0 = IS;I0<=N;I0+=ID) {
	 I1    = I0 + 1;
	 T1    = X[I0];
	 X[I0] = T1 + X[I1];
	 X[I1] = T1 - X[I1];
      }
      IS = 2 * ID - 1;
      ID *= 4;
   } while(IS<N);
/* ----L shaped butterflies----------------------------------------------- */
   sine = SINE;
   cosine = COSINE;
   
   for(N2=2,K=2;K<=M;K++) {
      N2    *= 2;
      N4    = N2/4;
/* ----Compute single stage of length N2 transform------------------------ */
      X1 = X;
      X2 = X1 + N4;
      X3 = X2 + N4;
      X4 = X3 + N4;
      N8    = N2/8;
      IS    = 0;
      ID    = N2 * 2;
	 
      do {
	 for (I1=IS+1;I1<=N;I1+=ID) {
	    T1 = X4[I1] + X3[I1];
	    X4[I1] = X4[I1] - X3[I1];
	    X3[I1] = X1[I1] - T1;
	    X1[I1] = X1[I1] + T1;
	 }
	 
	 IS = 2 * ID - N2;
	 ID *= 4;
      } while(IS<N);
      
      if (N4>1) {
	 IS = 0;
	 ID = N2 * 2;
	 do {
	    for (I2=IS+1+N8;I2<=N;I2+=ID) {
	       T1 = (X3[I2] + X4[I2]) *  (float) SQRT1_2;
	       T2 = (X3[I2] - X4[I2]) *  (float) SQRT1_2;
	       X4[I2] = X2[I2] - T1;
	       X3[I2] = -X2[I2] - T1;
	       X2[I2] = X1[I2] - T2;
	       X1[I2] = X1[I2] + T2;
	    }
	    
	    IS = 2 * ID - N2;
	    ID *= 4;
	 } while (IS<N);
      }
      else
	 continue;

      if (N8>1) {
//	 E = TWO_PI / N2;
//	 SS1 = sin(E);
	 SS1 = *sine++;
	 SD1 = SS1;
	 SD3 = (float) 3.0 * SS1 - (float) 4.0 * SD1 * SD1 * SD1;
	 SS3 = SD3;
//	 CC1 = cos(E);
	 CC1 = *cosine++;
	 CD1 = CC1;
	 CD3 = (float) 4.0 * CD1 * CD1 * CD1 - (float) 3.0 * CD1;
	 CC3 = CD3;

	 for (J=2; J<=N8; J++) {
	    IS = 0;
	    ID = 2 * N2;
	    JN = N4 - 2 * J + 2;
	    do {
	       for (I1=IS+J; I1<=N; I1+=ID) {
		  I2 = I1 + JN;
		  T1 = X3[I1] * CC1 + X3[I2] * SS1;
		  T2 = X3[I2] * CC1 - X3[I1] * SS1;
		  T3 = X4[I1] * CC3 + X4[I2] * SS3;
		  T4 = X4[I2] * CC3 - X4[I1] * SS3;
		  T5 = T1 + T3;
		  T3 = T1 - T3;
		  T1 = T2 + T4;
		  T4 = T2 - T4;
		  X3[I1]  = T1 - X2[I2];
		  X4[I2] = T1 + X2[I2];
		  X3[I2] = -X2[I1] - T3;
		  X4[I1] = X2[I1] - T3;
		  X2[I2] = X1[I1] - T5;
		  X1[I1] = X1[I1] + T5;
		  X2[I1] = X1[I2] + T4;
		  X1[I2] = X1[I2] - T4;
	       }
	       IS = 2 * ID - N2;
	       ID *= 4;
	    } while (IS<N);

	    T1 = CC1 * CD1 - SS1 * SD1;
	    SS1 = CC1 * SD1 + SS1 * CD1;
	    CC1 = T1;
	    T3 = CC3 * CD3 - SS3 * SD3;
	    SS3 = CC3 * SD3 + SS3 * CD3;
	    CC3 = T3;
	 }
      }
   }
}
