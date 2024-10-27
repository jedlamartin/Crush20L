/*
  ==============================================================================

    Stage6.cpp
    Created: 11 May 2024 4:45:47pm
    Author:  Martin

  ==============================================================================
*/

#include "Stage6.h"

void Stage6::processBlock(juce::AudioBuffer<float>& buffer){
    //y[k]=Ay[k-1]+Bu[k]+Cu[k-1]
    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, 0.0f);
    this->uBuffer.resize(channelNumber, 0.0f);

    for (int channel = 0; channel < channelNumber; channel++) {
        float* channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {

            if (std::abs(maxInput) < std::abs(channelSamples[i])) {
                maxInput = channelSamples[i];
            }

            float tmp = channelSamples[i];

            channelSamples[i] = A * yBuffer[channel] + B * channelSamples[i] + C * uBuffer[channel];
            this->yBuffer[channel] = channelSamples[i];
            this->uBuffer[channel] = tmp;

            if (std::abs(maxOutput) < std::abs(channelSamples[i])) {
                maxOutput = channelSamples[i];
            }
        }
    }

}

void Stage6::configure(double sampleRate){

    float T = static_cast<float>(1 / sampleRate);

    float G = 1 / (517 + 25000 * T);
    this->A = -(25000 * T - 517) * G;
    this->B = (24717 + 25000 * T) * G;
    this->C = (25000 * T - 24717) * G;
    maxInput = maxOutput = 0;
}
