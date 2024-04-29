/*
  ==============================================================================

    Stage2.cpp
    Created: 19 Apr 2024 8:45:49pm
    Author:  Martin

  ==============================================================================
*/

#include "Stage2.h"

void Stage2::configure(double sampleRate) {
    float T = 1 / sampleRate;
    float G1 = *this->gainParameter * 10000;
    float G2 = 10000 - G1;

    

}

void Stage2::initParameters(juce::AudioProcessorValueTreeState& vts) {
    this->gainParameter = vts.getRawParameterValue("gain");
}
