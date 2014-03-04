//
//  ScoreFollower.cpp
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/9/14.
//
//

#include "ScoreFollower.h"


ScoreFollower::ScoreFollower():playingThread("audio Input source")
{
    AudioDeviceManager::AudioDeviceSetup config;
    config.sampleRate = RAWSAMPLERATE;
    config.bufferSize = RECORDBUFFER;
    deviceManager.setAudioDeviceSetup(config, true);
    deviceManager.initialise(2, /* number of input channels */
                             2, /* number of output channels */
                             0, /* no XML settings*/
                             true, /* select default device on failure */
                             String::empty, /* preferred device name */
                             &config/* preferred setup options */);
    choice = 0;//FILE_INPUT;
    if (choice == FILE_INPUT)
    {
        formatManager.registerBasicFormats();
        audioSourcePlayer.setSource(&transportSource);
    }
    deviceManager.addAudioCallback(this);
    playingThread.startThread();


    ready = 0;
    inputToggle = 0;
    parseScoreandModel();
    
    //Follower = new Follower(score, probModel);
}

ScoreFollower::~ScoreFollower(){
    ready = false;
    deviceManager.removeAudioCallback(this);
    if(choice == FILE_INPUT)
    {
        transportSource.setSource(0);
        deleteAndZero(fileSource);
        audioSourcePlayer.setSource(0);
    }
    delete follower;
}

void ScoreFollower::audioDeviceIOCallback(const float** inputChannelData,
                           int totalNumInputChannels,
                           float** outputChannelData,
                           int totalNumOutputChannels,
                           int numSamples){
    if(choice == LIVE_INPUT)
    {
        for (int i = 0; i < numSamples; ++i)
            for (int j = totalNumOutputChannels; --j >= 0;)
                outputChannelData[j][i] = 0;        // clean up the out data
        if(ready)
            follower->followingMain(inputChannelData[0]);
    }
    else if(choice == FILE_INPUT)
    {
        audioSourcePlayer.audioDeviceIOCallback (inputChannelData, totalNumInputChannels, outputChannelData, totalNumOutputChannels, numSamples);   //pass the output to the player
        if(ready && inputToggle)
            follower->followingMain(outputChannelData[0]);
        
    }
    
    
}

void ScoreFollower::audioDeviceAboutToStart (AudioIODevice* device){
    if (choice == FILE_INPUT)
        audioSourcePlayer.audioDeviceAboutToStart (device);
    
}

void ScoreFollower::audioDeviceStopped(){
    if (choice == FILE_INPUT)
        audioSourcePlayer.audioDeviceStopped();
    
}


void ScoreFollower::parseScoreandModel(){
    vector<int> score;
    vector<vector<float> >probModel;
    File scoreFile("/Users/Toro/Documents/Spring2014/7100/ScoreFollowing/data/4_score.txt");
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
        temp.addTokens(models[i], ",", "");
        
        vector<float> oneDimension;
        for(int j = 0; j<temp.size(); j++){
            oneDimension.push_back(temp[j].getFloatValue());
        }
        probModel.push_back(oneDimension);
    }
    
    follower = new Follower(score,probModel);           // initialize the follower
    ready = true;
    
}

void ScoreFollower::setFile(File audioFile)
{
    if (choice == FILE_INPUT)
    {
        if(audioFile.exists())
        {
            AudioFormatReader* tempReader = formatManager.createReaderFor(audioFile);
            fileSource = new AudioFormatReaderSource(tempReader,true);
            transportSource.setSource(fileSource,32768,&playingThread,RAWSAMPLERATE);
            transportSource.start();
            inputToggle=true;
        }
    }
    
    if (choice == LIVE_INPUT)
    {
        
    }
}

