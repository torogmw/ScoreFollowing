//
//  Follower.cpp
//  ScoreFollowing
//
//  Created by Minwei Gu on 2/10/14.
//
//

#include "Follower.h"
Follower::Follower(vector<int>& score_, vector<vector<float> >& probModel_):sampleRate(16000),hopSize(256),frameSize(1024),score(score_),probModel(probModel_)
{
    raw = new float[frameSize*3];
    audioInput = new float[frameSize];
    rainBuffer = new float[hopSize];
    // chroma init
    chroma = new ChromaFeat(frameSize);
    chromahist = new float[NUMCHROMAGRAM];        // 12-d vectors
    memset(chromahist, 0, NUMCHROMAGRAM*sizeof(float));
    chromaBuffer = new float[frameSize*10];       // 10240 samples for chroma
    memset(chromaBuffer, 0, frameSize*10*sizeof(float));
    
    
    fft = new SplitRadixFFT(log2(frameSize));
    fftBuffer = new float[frameSize*2];
    for (unsigned i = 0; i<frameSize; i++) audioInput[i] = 0;   //init the audioBuffer to all 0
    for (unsigned i = 0; i<hopSize; i++) rainBuffer[i] = 0;     //init the rainBuffer to all 0
    for (unsigned i = 0; i<frameSize*2; i++) fftBuffer[i] = 0;  //init the fftBuffer to all 0
    featList.push_back(0);
    featList.push_back(0);
    featList.push_back(0);
    
    // init distribution matrix
    for (unsigned i = 0; i<probModel[0].size()-1; i++)
        dist.push_back(i*0.001);
    // init transtion matrix
    for (unsigned i = 0; i<3; i++)
    {
        vector<float> oneDimension;
        for (unsigned j = 0; j<3; j++)
        {
            if((i==0 && j ==2) || (i==2 && j==1))
                oneDimension.push_back(0);
            else
                oneDimension.push_back(1.0/7);
        }
        transMatrix.push_back(oneDimension);
    }
    // init the state
    currState = REST;
    currNote = 0;
    traCount = 0;
    tempPitch = 0;
    // for test use
    audioTest.open("/Users/Toro/Documents/Spring2014/7100/ScoreFollowing/test");
    
}

Follower::~Follower()
{
    delete[] audioInput;
    delete[] rainBuffer;
    delete[] fftBuffer;
    delete[] chromahist;
    delete[] chromaBuffer;
    delete fft;
    audioTest.close();
    cout<<"following finished! "<<endl;
}

void Follower::followingMain(const float* rawAudio)
{
    
    resample(rawAudio, 48000, sampleRate, hopSize*48000/sampleRate);
    // observation
    traCount++;
    observation();
    alignment();
}


void Follower::observation()
{
    getFeatures();
    pitchTracking();
}

void Follower::alignment()
{
    int aIndex = findMin(featList[0],dist);
    int sIndex = findMin(featList[1],dist);
    int rIndex = findMin(featList[2],dist);
    //cout<<"a: "<<aIndex<<" s: "<<sIndex<<" r: "<<rIndex<<endl;
    float attackProb = (probModel[0][aIndex]+probModel[3][aIndex]+probModel[6][aIndex])*transMatrix[currState][ATTACK];
    float susProb = (probModel[1][sIndex]+probModel[4][sIndex]+probModel[7][sIndex])*transMatrix[currState][SUSTAIN];
    float restProb = (probModel[3][rIndex]+probModel[5][rIndex]+probModel[8][rIndex])*transMatrix[currState][REST];

    if (attackProb>susProb && attackProb>restProb && currState!=ATTACK && currNote<score.size())
    {
        currState = ATTACK;
        if(abs(tempPitch-currNote)%NUMCHROMAGRAM)
        {
            cout<<score[currNote]<<endl;
            currNote++;
        }
        cout<<"attack! "<<endl;
    }
    else if(susProb>attackProb && susProb>restProb && currState!=SUSTAIN)
    {
         currState = SUSTAIN;
        //cout<<"sustain! "<<endl;
    }
    else if(restProb>attackProb && restProb>susProb && currState !=REST)
    {
        currState = REST;
        //cout<<"rest! "<<endl;
    }
    
    
}

int Follower::findMin(float key, vector<float> oneDimension)
{
    float min = 10000;
    int minIndex = 0;
    for (int i = 0; i<oneDimension.size()-1; i++)
    {
        if (abs(key-oneDimension[i])<min)
        {
            min = abs(key-oneDimension[i]);
            minIndex = i;
        }
    }
    return minIndex;
}
void Follower::getFeatures()
{
    int normIndex = probModel[0].size()-1;
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
    logEnergy = log2(logEnergy)/probModel[0][normIndex]/2;
    //spectral balance
    int fBound1 = frameSize/6;
    int fBound2 = frameSize/3;
    int fBound3 = frameSize/2;
    float fftSub1 = 0;
    float fftSub2 = 0;
    float fftSub3 = 0;
    float fftSum = 0;
    float specBalance = 0;
    for (int i = 0; i<fBound3; i++){
        if (i<=fBound1)
            fftSub1+=sqrt(fftBuffer[i]*fftBuffer[i]+fftBuffer[frameSize-i]*fftBuffer[frameSize-i]);
        else if(i>fBound1 && i<=fBound2)
            fftSub2+=sqrt(fftBuffer[i]*fftBuffer[i]*fftBuffer[frameSize-i]*fftBuffer[frameSize-i]);
        else
            fftSub3+=sqrt(fftBuffer[i]*fftBuffer[i]*fftBuffer[frameSize-i]*fftBuffer[frameSize-i]);
    }
    for (int i = 0; i<fBound3; i++)
    {
        fftSum+=fftBuffer[i]*fftBuffer[i]+fftBuffer[frameSize-i]+fftBuffer[frameSize-i];
    }
    specBalance = (fftSub1 - 2*fftSub2 + fftSub3)/sqrt(fftSum);
    specBalance = specBalance/probModel[3][normIndex];
    //ZCR
    int zcrCount = 0;
    float zcr = 0;
    for (int i = 1; i<frameSize; i++){
        if(audioInput[i]*audioInput[i-1]<0)
            zcrCount+=1;
    }
    zcr= (zcrCount*1.0/frameSize)/probModel[6][normIndex];
    featList[0] = logEnergy;
    featList[1] = specBalance;
    featList[2] = zcr;
    
}

void Follower::pitchTracking()
{
    // we are using chroma buffer here
    if(traCount%4)
    {
        for (int i=0; i<NUMCHROMAGRAM; i++)
        chromaFreq.push_back(0);
        int numChunks = floor((float)(frameSize*9)*1.0/hopSize);
        targetEnergyPeaks.clear();
        for (int i=0; i<numChunks; i++){
            memset(chroma->chromagram, 0, NUMCHROMAGRAM*sizeof(float));
            chroma->Chroma(&chromaBuffer[i*hopSize]);
            for (int j=0; j<NUMCHROMAGRAM; j++)
                chromahist[j] = chroma->chromagram[j];
            normalize(chromahist, NUMCHROMAGRAM);
            int peakBin = maxIdx(chromahist, NUMCHROMAGRAM);;
            chromaFreq[peakBin] += 1;
            targetEnergyPeaks.push_back(maxIdx(chroma->energysum, NUMNOTES));
        }
        smooth(targetEnergyPeaks);
        for (int i=0; i<numChunks; i++)
             tempPitch = targetEnergyPeaks[i]+STARTNOTE;
    }
    //cout<<tempPitch<<endl;
}

void Follower::resample(const float *rawInput, int in_rate, int out_rate, long in_length)
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
        // change this
        if(result <= -32768) rainBuffer[i] = -32768;
        else if(result > 32767) rainBuffer[i] = 32767;
        rainBuffer[i] = result;
        //audioTest<<rainBuffer[i]<<" ";
    }
    
    
    // brutal force move forward
    for (unsigned i = 0; i<frameSize; i++){
        if (i <frameSize - hopSize)
            audioInput[i] = audioInput[i+hopSize];
        else
            audioInput[i] = rainBuffer[i+hopSize-frameSize];
    }
    
    // buratal force move forward
    for (unsigned i = 0; i<frameSize*10; i++){
        if(i<frameSize*10-hopSize)
            chromaBuffer[i] = chromaBuffer[i+hopSize];
        else
            chromaBuffer[i] = audioInput[i-frameSize*10+hopSize];
    }
    if(traCount==40)
    {
         for (unsigned i = 0; i<frameSize*10; i++)
             //audioTest<<chromaBuffer[i]<<" "<<endl;
         traCount = 0;
        
    }
    
}

double Follower::sinc(double x)
{
    if(x == 0.0) return 1.0;
    x *= PI;
    return std::sin(x) / x;
}

void Follower::normalize(float* v, int size)
{
    float sum = 0;
    for (int i=0; i<size; i++)
        sum += v[i]*v[i];
    sum = pow(sum, float(0.5));
    for (int i=0; i<size; i++)
        v[i] /= sum;
}

void Follower::smooth(vector<int> &c)
{
    for (int i=1; i<c.size()-1; i++)
        if(c[i-1] == c[i+1])
            c[i] = c[i-1];
}

int Follower::maxIdx(float *data, int size)
{
    float max = data[0];
    int idx = 0;
    for (int i=1; i<size; i++){
        if (data[i] > max){
            max = data[i];
            idx = i;
        }
    }
    return idx;
}


