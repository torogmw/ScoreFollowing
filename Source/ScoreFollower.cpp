//
//  ScoreFollower.cpp
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/9/14.
//
//

#include "ScoreFollower.h"


ScoreFollower::ScoreFollower(){
;
    AudioDeviceManager::AudioDeviceSetup config;
    config.sampleRate = SAMPLERATE;
    config.bufferSize = RECORDBUFFER;
    deviceManager.setAudioDeviceSetup(config, true);
    deviceManager.initialise(2, /* number of input channels */
                             2, /* number of output channels */
                             0, /* no XML settings*/
                             true, /* select default device on failure */
                             String::empty, /* preferred device name */
                             &config/* preferred setup options */);

    deviceManager.addAudioCallback(this);
    parseScoreandModel();

    
    
    //Follower = new Follower(score, probModel);
}

ScoreFollower::~ScoreFollower(){
    delete follower;
}

void ScoreFollower::audioDeviceIOCallback(const float** inputChannelData,
                           int totalNumInputChannels,
                           float** outputChannelData,
                           int totalNumOutputChannels,
                           int numSamples){
    for (int i = 0; i < numSamples; ++i)
        for (int j = totalNumOutputChannels; --j >= 0;)
            outputChannelData[j][i] = 0;        // clean up the out data
    // rain buffer hard code
//    copyBuffer.copyFrom(0, 0, inputChannelData[0], numSamples);
//    tempBuffer.copyFrom(0, 0, feedBuffer, 0, numSamples, RECORDSIZE - numSamples);
//    tempBuffer.copyFrom(0, RECORDSIZE - numSamples, copyBuffer, 0, 0, numSamples);
//    feedBuffer.clear();
//    feedBuffer.copyFrom(0, 0, tempBuffer, 0, 0,RECORDSIZE);
//    copyBuffer.clear();
//    tempBuffer.clear();
    // rain buffere hard code
    

    follower->followingMain(inputChannelData[0]);
        
    
}

void ScoreFollower::audioDeviceAboutToStart (AudioIODevice* device){
    
    
}

void ScoreFollower::audioDeviceStopped(){
    
    
}


void ScoreFollower::parseScoreandModel(){
    vector<int> score;
    vector<vector<float> >probModel;
    File scoreFile("/Users/Toro/Documents/Spring2014/7100/ScoreFollowing/data/1_score.txt");
    StringArray scores;
    scoreFile.readLines(scores);
    for (int i = 0; i<scores.size(); i++)
    {
        StringArray temp;
        temp.addTokens(scores[i], ",");
        score.push_back(temp[0].getIntValue());
    }
    
    File modelFile("/Users/Toro/Documents/Spring2014/7100/ScoreFollowing/data/model.csv");
    StringArray models;
    modelFile.readLines(models);
    for (int i = 0; i<models.size(); i++)
    {
        StringArray temp;
        temp.addTokens(models[i], ",");
        vector<float> oneDimension;
        for(int j = 0; j<temp.size(); j++){
            oneDimension.push_back(temp[i].getFloatValue());
        }
        probModel.push_back(oneDimension);
    }
    
    follower = new Follower(score,probModel);           // initialize the follower 
}

