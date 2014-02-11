//
//  Follower.cpp
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/10/14.
//
//

#include "Follower.h"
Follower::Follower(vector<int>& score_, vector<vector<float> >& probModel_):score(score_),probModel(probModel_)
{
    audioInput = new float[frameSize];
    rainBuffer = new float[hopSize];
    fft = new SplitRadixFFT(log2(frameSize));
    fftBuffer = new float[frameSize*2];
    for (unsigned i = 0; i<frameSize; i++) audioInput[i] = 0;   //init the audioBuffer to all 0
    for (unsigned i = 0; i<hopSize; i++) rainBuffer[i] = 0;     //init the rainBuffer to all 0
    for (unsigned i = 0; i<frameSize*2; i++) fftBuffer[i] = 0;  //init the fftBuffer to all 0
    
    // for test use
    audioTest.open("/Users/Toro/Documents/Spring2014/7100/ScoreFollowing/test");
    
}

Follower::~Follower()
{
    delete[] audioInput;
    delete[] rainBuffer;
    delete[] fftBuffer;
    delete fft;
    audioTest.close();
    cout<<"following finished! "<<endl;
}

void Follower::followingMain(const float *rawAudio)
{
    resample(rawAudio, 48000, sampleRate, hopSize*48000/sampleRate);
    // observation
    getFeatures();
}


void Follower::observation()
{
    
}

void Follower::alignment()
{
    
}

void Follower::getFeatures()
{
    // first doing fft
    for (int j=0; j<frameSize; j++)
        fftBuffer[j] = audioInput[j];
    for (int j=frameSize; j<frameSize*2; j++)
        fftBuffer[j] = 0;
    fft->XForm(fftBuffer);
    //log energy
    float logEnergy = 0;
    for (int i = 0; i<frameSize; i++){
        logEnergy+=sqrt(fftBuffer[i] * fftBuffer[i] + fftBuffer[frameSize-i] *fftBuffer[frameSize-i]);
    }
    int normIndex = probModel[0].size()-1;
    logEnergy = log2(logEnergy)/probModel[0][normIndex];
    
    //spectral balance
    
    //ZCR
    
}


void Follower::resample(const float *rawInput, unsigned int in_rate, unsigned int out_rate, long in_length)
{
    double ratio         = in_rate / (double)out_rate;
    unsigned out_length  = std::ceil(in_length / ratio);
    const double support = 4.0;
    
    
    for(unsigned i=0; i<out_length; ++i)
    {
        double center = i * ratio;
        double min    = center-support;
        double max    = center+support;
        
        unsigned min_in = std::max( 0,                 (int)(min + 0.5) );
        unsigned max_in = std::min( (int)in_length-1,  (int)(max + 0.5) );
        double sum    = 0.0;
        double result = 0.0;
        for(unsigned i=min_in; i<=max_in; ++i)
        {
            double factor = sinc(i-center);
            result += rawInput[i] * factor;
            sum    += factor;
        }
        if(sum != 0.0) result /= sum;
        if(result <= -32768) rainBuffer[i] = -32768;
        else if(result > 32767) rainBuffer[i] = 32767;
        rainBuffer[i] = result + 0.5;
        //audioTest<<rainBuffer[i]<<" ";
    }
    
    
    // move forward
    for (unsigned i = 0; i<frameSize; i++){
        if (i <frameSize - hopSize)
            audioInput[i] = audioInput[i+hopSize];
        else
            audioInput[i] = rainBuffer[i+hopSize-frameSize];
    }
    
}

double Follower::sinc(double x)
{
    if(x == 0.0) return 1.0;
    x *= PI;
    return std::sin(x) / x;
}