/*
  ==============================================================================

    Stage5.cpp
    Created: 11 May 2024 12:49:03pm
    Author:  Martin

  ==============================================================================
*/

#include "Stage5.h"
#ifdef RESAMPLE
Stage5::Stage5():Stage(true){}
#else
Stage5::Stage5() : Stage(false) {}
#endif

void Stage5::processBlock(juce::AudioBuffer<float>& buffer){
    if (!crossfade.isSmoothing()) {
        ParameterSet5 updt;
        if (updatedParameters.read_and_pop(updt)) {
            //DBG("Changed!\n");
            if (crossfade.getTargetValue() == 1.0f) {
                updatedParams = updt;
                yUBuffer.clear();
                uUBuffer.clear();
                yUNegBuffer.clear();
                uUNegBuffer.clear();
                yUVolBuffer.clear();
                uUVolBuffer.clear();
                crossfade.setTargetValue(0.0f);
            }
            else {
                params = updt;
                yBuffer.clear();
                uBuffer.clear();
                yNegBuffer.clear();
                uNegBuffer.clear();
                yVolBuffer.clear();
                uVolBuffer.clear();
                crossfade.setTargetValue(1.0f);
            }
        }
    }

    //y[k]=Ay[k-1]+By[k-2]+Cy[k-3]+Dy[k-4]+Eu[k]+Fu[k-1]...
    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->uBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->yNegBuffer.resize(channelNumber, 0.0f);
    this->uNegBuffer.resize(channelNumber, 0.0f);
    this->yVolBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->uVolBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->yUBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->uUBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->yUNegBuffer.resize(channelNumber, 0.0f);
    this->uUNegBuffer.resize(channelNumber, 0.0f);
    this->yUVolBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->uUVolBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});

    for (int channel = 0; channel < channelNumber; channel++) {
        float* channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float yCurrent = params.A * yBuffer[channel][0] + params.B * yBuffer[channel][1] + params.C * channelSamples[i] + params.D * uBuffer[channel][0] + params.E * uBuffer[channel][1];
            float yUpdate = updatedParams.A * yUBuffer[channel][0] + updatedParams.B * yUBuffer[channel][1] + updatedParams.C * channelSamples[i] + updatedParams.D * uUBuffer[channel][0] + updatedParams.E * uUBuffer[channel][1];

            this->yBuffer[channel].push(yCurrent);
            this->uBuffer[channel].push(channelSamples[i]);
            this->yUBuffer[channel].push(yUpdate);
            this->uUBuffer[channel].push(channelSamples[i]);




            float tmp = yCurrent;
            float tmpU = yUpdate;
            yCurrent = params.F * yNegBuffer[channel] + params.G * yCurrent + params.H * uNegBuffer[channel];
            yUpdate = updatedParams.F * yUNegBuffer[channel] + updatedParams.G * yUpdate + updatedParams.H * uUNegBuffer[channel];

            this->yNegBuffer[channel] = yCurrent;
            this->uNegBuffer[channel] = tmp;
            this->yUNegBuffer[channel] = yUpdate;
            this->uUNegBuffer[channel] = tmpU;


            tmp = yCurrent;
            tmpU = yUpdate;
            yCurrent = params.I * yVolBuffer[channel][0] + params.J * yVolBuffer[channel][1] + params.K * yCurrent + params.L * uVolBuffer[channel][0] + params.M * uVolBuffer[channel][1];
            yUpdate = updatedParams.I * yUVolBuffer[channel][0] + updatedParams.J * yUVolBuffer[channel][1] + updatedParams.K * yUpdate + updatedParams.L * uUVolBuffer[channel][0] + updatedParams.M * uUVolBuffer[channel][1];
            
            this->yVolBuffer[channel].push(yCurrent);
            this->uVolBuffer[channel].push(tmp);
            this->yUVolBuffer[channel].push(yUpdate);
            this->uUVolBuffer[channel].push(tmpU);

            float mult = this->crossfade.getCurrentValue();
            channelSamples[i] = mult * yCurrent + (1.0f - mult) * yUpdate;
            this->crossfade.getNextValue();
        }
    }

}


void Stage5::configure(double sampleRate){

    this->sampleRate = static_cast<float>(sampleRate) * (this->resampled ? INT_SIZE : 1);
    float T = 1 / this->sampleRate;
    float V2 = *this->volParameter;
    float V1 = 50000.0f + 0.01f - V2;

    ParameterSet5 newValues;

    float G = 1 / (184467.44073709551616000000 * T * T + 8116.93632731367731603232 * T + 0.01623313478486440625);
    newValues.A = -(368934.88147419103232000000 * T * T - 0.03246626956972881250) * G;
    newValues.B = -(184467.44073709551616000000 * T * T - 8116.93632731367731603232 * T + 0.01623313478486440625) * G;
    newValues.C = (184467.44073709551616000000 * T * T + 16233.50371974588044103232 * T + 0.01623313478486440625) * G;
    newValues.D = (368934.88147419103232000000 * T * T - 0.03246626956972881250) * G;
    newValues.E = (184467.44073709551616000000 * T * T - 16233.50371974588044103232 * T + 0.01623313478486440625) * G;

    G = 1 / (517 + 25000000 * T);
    newValues.F = -(25000000 * T - 517) * G;
    newValues.G = (-25000000 * T) * G;
    newValues.H = (-25000000 * T) * G;


    G = 1 / (976585388.65744263019233280000 * T + 4296.97571009274828800000 * V1 + 4296.97571009274828800000 * V2 + 47223664828.69645213696000000000 * T * T + 4722366.48286964521369600000 * T * T * V1 + 4722366.48286964521369600000 * T * T * V2 + 305442.66411200865242185728 * T * V1 + 326221.07663663509136211968 * T * V2 + 0.09142501510835634375 * V1 * V2 + 2.07784125246264400000 * T * V1 * V2);
    newValues.I = -(-8593.95142018549657600000 * V1 - 8593.95142018549657600000 * V2 + 94447329657.39290427392000000000 * T * T - 0.18285003021671268750 * V1 * V2 + 9444732.96573929042739200000 * T * T * V1 + 9444732.96573929042739200000 * T * T * V2) * G;
    newValues.J = -(4296.97571009274828800000 * V1 - 976585388.65744263019233280000 * T + 4296.97571009274828800000 * V2 - 305442.66411200865242185728 * T * V1 - 326221.07663663509136211968 * T * V2 + 0.09142501510835634375 * V1 * V2 + 4722366.48286964521369600000 * T * T * V1 + 4722366.48286964521369600000 * T * T * V2 + 47223664828.69645213696000000000 * T * T - 2.07784125246264400000 * T * V1 * V2) * G;
    newValues.K = (4296.97571009274828800000 * V2) * G;
    newValues.L = (-8593.95142018549657600000 * V2) * G;
    newValues.M = (4296.97571009274828800000 * V2) * G;

    this->updatedParameters.push(newValues);

}

void Stage5::initParameters(std::atomic<float>* volParameter){
    this->volParameter = volParameter;
    this->crossfade.reset(CROSSFADE_SAMPLES);
}
