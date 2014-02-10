/* ----------------------------------------------------------- */
/*    File: rsrfft.h  Real-valued, split-radix FFT             */
/*    Date: 06/10/96                                           */
/*    Author: Kai Huebener                                     */
/* ----------------------------------------------------------- */

#ifndef RSRFFT_H
#define RSRFFT_H

/*=================================================================*/
/*                                                                 */
/*  Subroutine RSRFFT(X,M):                                        */
/*      A real-valued, in-place, split-radix FFT program           */
/*      Decimation-in-time, cos/sin in second loop                 */
/*      and computed recursively                                   */
/*      Output in order:                                           */
/*              [ Re(0),Re(1),....,Re(N/2),Im(N/2-1),...Im(1)]     */
/*                                                                 */
/*  Input/output                                                   */
/*      X    Array of input/output (length >= N)                   */
/*      M    Transform length is N=2**M                            */
/*                                                                 */
/*  Calls:                                                         */
/*      RSTAGE,RBITREV                                             */
/*                                                                 */
/*  Author:                                                        */
/*      H.V. Sorensen,   University of Pennsylvania,  Oct. 1985    */
/*                       Arpa address: hvs@ee.upenn.edu            */
/*  Modified:                                                      */
/*      F. Bonzanigo,    ETH-Zurich,                  Sep. 1986    */
/*      H.V. Sorensen,   University of Pennsylvania,  Mar. 1987    */
/*      H.V. Sorensen,   University of Pennsylvania,  Oct. 1987    */
/*                                                                 */
/*  Reference:                                                     */
/*      Sorensen, Jones, Heideman, Burrus :"Real-valued fast       */
/*      Fourier transform algorithms", IEEE Tran. ASSP,            */
/*      Vol. ASSP-35, No. 6, pp. 849-864, June 1987                */
/*      Mitra&Kaiser: "Digital Signal Processing Handbook, Chap.   */
/*      8, page 491-610, John Wiley&Sons, 1993                     */
/*                                                                 */
/*      This program may be used and distributed freely as         */
/*      as long as this header is included                         */
/*                                                                 */
/*=================================================================*/

class SplitRadixFFT {
private:
   const int M;			// transform order
   const int N;			// transform length
   const int NBIT;
   float *SINE, *COSINE;
   int *ITAB;
   
public:
   SplitRadixFFT(const int order);
   ~SplitRadixFFT();

   void XForm(float *real);
};

#endif
