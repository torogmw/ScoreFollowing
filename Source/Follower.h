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
    void resample(float* rawInput, unsigned in_rate, unsigned out_rate, long in_length);
    // feature extraction family
    void featZCR(vector<float>& tempData, int fileIdx);
    void featEnv(vector<float>& tempData, int fileIdx);
    void featCentroid(vector<float>& tempData, int fileIdx);
    void featRMS(vector<float>& tempData, int fileIdx);
    // hmm model
    
private:
    const int sampleRate = 16000;              // restrict the samplerate to 160000
    const int frameSize = 1024;
    const int hopSize = 100;
    vector<int> score;                         // repreent the score in a vector
    vector<vector<float> > probModel;          // probablistic stored in an 2d array
    vector<float> featList;
    int currPos;
    float* audioInput;
};

#endif /* defined(__ScoreFollowing__Follower__) */
