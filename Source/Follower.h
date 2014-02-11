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
using namespace std;

#define ATTACK     1
#define SUSTAIN    2
#define REST       3


class Follower{
public:
    Follower(vector<int>& score, vector<vector<float> >& probModel);
    ~Follower();
    void observation();
    void alignment();
    void followingMain(const float* rawAudio);
    void resample(const float* rawInput, unsigned in_rate, unsigned out_rate, long in_length);
    // feature extraction family
    void featZCR(float* audioInput, int fileIdx);
    void featEnv(float* audioInput, int fileIdx);
    void featCentroid(float* audioInput, int fileIdx);
    void featRMS(float* audioInput, int fileIdx);
    
    // hmm model
    // math function
    double sinc(double x);
    
private:
    const int sampleRate = 16000;              // restrict the samplerate to 160000
    const int frameSize = 1024;                // the frameSize is for downsampled signal
    const int hopSize = 256;                   // hopSize is a quarter of the signal
    const double PI = 3.1415926;
    vector<int> score;                         // repreent the score in a vector
    vector<vector<float> > probModel;          // probablistic stored in an 2d array
    vector<float> featList;
    int currPos;
    float* audioInput;
    float* rainBuffer;
    int overlapCount;
    ofstream audioTest;
};

#endif /* defined(__ScoreFollowing__Follower__) */
