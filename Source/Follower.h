//
//  Follower.h
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/10/14.
//
//

#ifndef __ScoreFollowing__Follower__
#define __ScoreFollowing__Follower__

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "rsrfft.h"
using namespace std;

#define ATTACK     0
#define SUSTAIN    1
#define REST       2

#define SAMPLERATE 16000
#define FRAMESIZE 1024
#define HOPSIZE 256
#define PI 3.1415926

class Follower{
public:
    Follower(vector<int>& score, vector<vector<float> >& probModel);
    ~Follower();
    void observation();
    void alignment();
    void followingMain(const float* rawAudio);
    void resample(const float* rawInput, int in_rate, int out_rate, long in_length);
    // feature extraction family
    void getFeatures();
    
    // hmm model, transition matrix and veterbi algorith,
    vector<vector <float> > transMatrix;

    // math function
    double sinc(double x);
    int findMin(float key, vector<float> oneDimension);
private:
    const int sampleRate;              // restrict the samplerate to 160000
    const int frameSize;                // the frameSize is for downsampled signal
    const int hopSize;                   // hopSize is a quarter of the signal
    //const double PI = 3.1415926;
    vector<int> score;                         // repreent the score in a vector
    vector<vector<float> > probModel;          // probablistic stored in an 2d array
    vector<float> featList;
    vector<float> dist;
    int currState;
    float* raw;
    float* audioInput;
    float* rainBuffer;
    SplitRadixFFT* fft;
    float* fftBuffer;
    ofstream audioTest;
};

#endif /* defined(__ScoreFollowing__Follower__) */
