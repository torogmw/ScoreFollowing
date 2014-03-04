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

#define RAWSAMPLERATE   48000
#define RECORDSIZE   3072
#define RECORDBUFFER 768
#define LIVE_INPUT 0
#define FILE_INPUT 1

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
    void setFile(File audioFile);
    void stopPlaying();
    bool setToggle();

    
private:
    AudioDeviceManager deviceManager;
    Follower* follower;
    bool ready;
    //AudioDeviceManager& deviceManager;
    AudioSourcePlayer audioSourcePlayer;
    AudioFormatReaderSource* fileSource;
    AudioTransportSource transportSource;
    AudioFormatManager formatManager;
    TimeSliceThread playingThread;
    int choice;
    bool inputToggle;

    //int bufferCount; //count every 1.5 times for parse
    AudioSampleBuffer copyBuffer = AudioSampleBuffer(1, RECORDBUFFER);
    AudioSampleBuffer tempBuffer = AudioSampleBuffer(1, RECORDSIZE); // due to the sample rate
    AudioSampleBuffer feedBuffer = AudioSampleBuffer(1, RECORDSIZE); // due to the sample rate
    
};