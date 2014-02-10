//
//  ScoreFollower.cpp
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/9/14.
//
//

#include "ScoreFollower.h"

ScoreFollower::ScoreFollower(){
    deviceManager.initialise(2, /* number of input channels */
                             2, /* number of output channels */
                             0, /* no XML settings*/
                             true, /* select default device on failure */
                             String::empty, /* preferred device name */
                             0 /* preferred setup options */);
    deviceManager.addAudioCallback(this);
    
}

ScoreFollower::~ScoreFollower(){
    
}

void ScoreFollower::audioDeviceIOCallback(const float** inputChannelData,
                           int totalNumInputChannels,
                           float** outputChannelData,
                           int totalNumOutputChannels,
                           int numSamples){
 
    std::cout<<*inputChannelData[0]<<std::endl;
}

void ScoreFollower::audioDeviceAboutToStart (AudioIODevice* device){
    
    
}

void ScoreFollower::audioDeviceStopped(){
    
    
}


