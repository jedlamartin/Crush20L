#include "Stage6.h"

Stage6::Stage6():Stage(true), cutOffVoltage(24.0f){}

void Stage6::processBlock(juce::AudioBuffer<float>& buffer){
    //y[k]=Ay[k-1]+Bu[k]+Cu[k-1]
    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, 0.0f);
    this->uBuffer.resize(channelNumber, 0.0f);

    for (int channel = 0; channel < channelNumber; channel++) {
        float* channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {

            float tmp = channelSamples[i];

            channelSamples[i] = A * yBuffer[channel] + B * channelSamples[i] + C * uBuffer[channel];
            
            //Cutoff
            if (channelSamples[i] > this->cutOffVoltage) {
                channelSamples[i] = this->cutOffVoltage;
                tmp = (this->cutOffVoltage - A * yBuffer[channel] - C * uBuffer[channel]) / B;
            }
            else if (channelSamples[i] < -this->cutOffVoltage) {
                channelSamples[i] = -this->cutOffVoltage;
                tmp = (-this->cutOffVoltage - A * yBuffer[channel] - C * uBuffer[channel]) / B;
            }
            
            
            
            this->yBuffer[channel] = channelSamples[i];
            this->uBuffer[channel] = tmp;

        }
    }

}

void Stage6::configure(double sampleRate){

    this->sampleRate = static_cast<float>(sampleRate) * (this->resampled ? INT_SIZE : 1);
    float T = 1 / this->sampleRate;

    float G = 1 / (517 + 25000 * T);
    this->A = -(25000 * T - 517) * G;
    this->B = (24717 + 25000 * T) * G;
    this->C = (25000 * T - 24717) * G;
}
