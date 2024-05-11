/*
  ==============================================================================

    Stage5.cpp
    Created: 11 May 2024 12:49:03pm
    Author:  Martin

  ==============================================================================
*/

#include "Stage5.h"

void Stage5::processBlock(juce::AudioBuffer<float>& buffer){
    //y[k]=Ay[k-1]+By[k-2]+Cy[k-3]+Dy[k-4]+Eu[k]+Fu[k-1]...
    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, std::array<float, 2>{0.0f});
    this->uBuffer.resize(channelNumber, std::array<float, 2>{0.0f});
    this->yNegBuffer.resize(channelNumber, 0.0f);
    this->uNegBuffer.resize(channelNumber, 0.0f);
    this->yVolBuffer.resize(channelNumber, std::array<float, 2>{0.0f});
    this->uVolBuffer.resize(channelNumber, std::array<float, 2>{0.0f});

    for (int channel = 0; channel < channelNumber; channel++) {
        float* channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float tmp = channelSamples[i];
            channelSamples[i] = A * yBuffer[channel][0] + B * yBuffer[channel][1] + C * channelSamples[i] + D * uBuffer[channel][0] + E * uBuffer[channel][1];

            this->yBuffer[channel][1] = this->yBuffer[channel][0];
            this->yBuffer[channel][0] = channelSamples[i];
            this->uBuffer[channel][1] = this->uBuffer[channel][1];
            this->uBuffer[channel][0] = tmp;




            tmp = channelSamples[i];
            channelSamples[i] = F * yNegBuffer[channel] + G * channelSamples[i] + H * uNegBuffer[channel];

            this->yNegBuffer[channel] = channelSamples[i];
            this->uNegBuffer[channel] = tmp;




            tmp = channelSamples[i];
            channelSamples[i] = I * yVolBuffer[channel][0] + J * yVolBuffer[channel][1] + K * channelSamples[i] + L * uVolBuffer[channel][0] + M * uVolBuffer[channel][1];
            
            this->yVolBuffer[channel][1] = this->yVolBuffer[channel][0];
            this->yVolBuffer[channel][0] = channelSamples[i];
            this->uVolBuffer[channel][1] = this->uVolBuffer[channel][0];
            this->uVolBuffer[channel][0] = tmp;
        }
    }

}


void Stage5::configure(double sampleRate){
    float T = static_cast<float>(1 / sampleRate);
    float V2 = *this->volParameter;
    float V1 = 50000.0f + 0.01f - V2;

    float G = 1 / (184467.44073709551616000000 * T * T + 8116.93632731367731603232 * T + 0.01623313478486440625);
    this->A = -(368934.88147419103232000000 * T * T - 0.03246626956972881250) * G;
    this->B = -(184467.44073709551616000000 * T * T - 8116.93632731367731603232 * T + 0.01623313478486440625) * G;
    this->C = (184467.44073709551616000000 * T * T + 16233.50371974588044103232 * T + 0.01623313478486440625) * G;
    this->D = (368934.88147419103232000000 * T * T - 0.03246626956972881250) * G;
    this->E = (184467.44073709551616000000 * T * T - 16233.50371974588044103232 * T + 0.01623313478486440625) * G;

    G = 1 / (517 + 25000000 * T);
    this->F = -(25000000 * T - 517) * G;
    this->G = (-25000000 * T) * G;
    this->H = (-25000000 * T) * G;


    G = 1 / (976585388.65744263019233280000 * T + 4296.97571009274828800000 * V1 + 4296.97571009274828800000 * V2 + 47223664828.69645213696000000000 * T * T + 4722366.48286964521369600000 * T * T * V1 + 4722366.48286964521369600000 * T * T * V2 + 305442.66411200865242185728 * T * V1 + 326221.07663663509136211968 * T * V2 + 0.09142501510835634375 * V1 * V2 + 2.07784125246264400000 * T * V1 * V2);
    this->I = -(-8593.95142018549657600000 * V1 - 8593.95142018549657600000 * V2 + 94447329657.39290427392000000000 * T * T - 0.18285003021671268750 * V1 * V2 + 9444732.96573929042739200000 * T * T * V1 + 9444732.96573929042739200000 * T * T * V2) * G;
    this->J = -(4296.97571009274828800000 * V1 - 976585388.65744263019233280000 * T + 4296.97571009274828800000 * V2 - 305442.66411200865242185728 * T * V1 - 326221.07663663509136211968 * T * V2 + 0.09142501510835634375 * V1 * V2 + 4722366.48286964521369600000 * T * T * V1 + 4722366.48286964521369600000 * T * T * V2 + 47223664828.69645213696000000000 * T * T - 2.07784125246264400000 * T * V1 * V2) * G;
    this->K = (4296.97571009274828800000 * V2) * G;
    this->L = (-8593.95142018549657600000 * V2) * G;
    this->M = (4296.97571009274828800000 * V2) * G;



}

void Stage5::initParameters(std::atomic<float>* volParameter){
    this->volParameter = volParameter;
}
