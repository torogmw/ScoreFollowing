/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/
#include <iostream>
#include "JuceHeader.h"
#include "ScoreFollower.h"

//==============================================================================
int main (int argc, char* argv[])
{
    // ..your code goes here!
    char stop;
    ScoreFollower* test = new ScoreFollower();
    scanf ("press any key to end this instance: %s", &stop);
    delete test;
    return 0;
}
