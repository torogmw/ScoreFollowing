//
//  ScoreFollower.h
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/9/14.
//
//

#ifndef __ScoreFollowing__ScoreFollower__
#define __ScoreFollowing__ScoreFollower__

#include <iostream>
#include <vector>
#include "JuceHeader.h"
#include "Follower.h"

#define SAMPLERATE   48000
#define RECORDSIZE   3072
#define RECORDBUFFER 768

using namespace std;
#endif /* defined(__ScoreFollowing__ScoreFollower__) */

class ScoreFollower : public AudioIODeviceCallback{
public:
    ScoreFollower();
    ~ScoreFollower();
    void audioDeviceIOCallback(const float** inputChannelData,
							   int totalNumInputChannels,
							   float** outputChannelData,
							   int totalNumOutputChannels,
							   int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void parseScoreandModel();
    
private:
    AudioDeviceManager deviceManager;
    Follower* follower;
    bool ready;
    //int bufferCount; //count every 1.5 times for parse
    AudioSampleBuffer copyBuffer = AudioSampleBuffer(1, RECORDBUFFER);
    AudioSampleBuffer tempBuffer = AudioSampleBuffer(1, RECORDSIZE); // due to the sample rate
    AudioSampleBuffer feedBuffer = AudioSampleBuffer(1, RECORDSIZE); // due to the sample rate
    
};