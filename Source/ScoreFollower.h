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
#include "JuceHeader.h"
#include <vector>

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

private:
    AudioDeviceManager deviceManager;
    
    
};