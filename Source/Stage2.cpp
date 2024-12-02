/*
  ==============================================================================

    Stage2.cpp
    Created: 19 Apr 2024 8:45:49pm
    Author:  Martin

  ==============================================================================
*/

#include "Stage2.h"

#ifdef RESAMPLE
    Stage2::Stage2() :Stage(true), cutOffVoltage(TL072_CUTOFF) {}
#else
    Stage2::Stage2() : Stage(false), cutOffVoltage(TL072_CUTOFF) {}
#endif


void Stage2::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (!crossfade.isSmoothing()) {
        ParameterSet2 updt;
        if (updatedParameters.read_and_pop(updt)) {
            if (crossfade.getTargetValue() == 1.0f) {
                updatedParams = updt;
                yUBuffer.clear();
                uUBuffer.clear();
                yURCBuffer.clear();
                uURCBuffer.clear();
                crossfade.setTargetValue(0.0f);
            }
            else {
                params = updt;
                yBuffer.clear();
                uBuffer.clear();
                yRCBuffer.clear();
                uRCBuffer.clear();
                crossfade.setTargetValue(1.0f);
            }
        }
    }


    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->uBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->yRCBuffer.resize(channelNumber, 0.0f);
    this->uRCBuffer.resize(channelNumber, 0.0f);
    this->yUBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->uUBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
    this->yURCBuffer.resize(channelNumber, 0.0f);
    this->uURCBuffer.resize(channelNumber, 0.0f);

    for (auto channel = 0; channel < channelNumber; channel++) {
        float* channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float tmp = channelSamples[i];
            float tmpU = channelSamples[i];


            float yCurrent = params.A * yBuffer[channel][0] + params.B * yBuffer[channel][1] + params.C * channelSamples[i] + params.D * uBuffer[channel][0] + params.E * uBuffer[channel][1];
            float yUpdate = updatedParams.A * yUBuffer[channel][0] + updatedParams.B * yUBuffer[channel][1] + updatedParams.C * channelSamples[i] + updatedParams.D * uUBuffer[channel][0] + updatedParams.E * uUBuffer[channel][1];
            

            if (std::abs(yCurrent) > std::abs(this->maxBetween) || std::abs(yUpdate) > std::abs(this->maxBetween))
                this->maxBetween = yCurrent > yUpdate ? yCurrent : yUpdate;

            //Cutoff
            if (yCurrent > this->cutOffVoltage) {
                yCurrent = this->cutOffVoltage;
                tmp = (this->cutOffVoltage - params.A * yBuffer[channel][0] - params.B * yBuffer[channel][1] - params.D * uBuffer[channel][0] - params.E * uBuffer[channel][1]) / params.C;
            }
            else if (yCurrent < -this->cutOffVoltage) {
                yCurrent = -this->cutOffVoltage;
                tmp = (-this->cutOffVoltage - params.A * yBuffer[channel][0] - params.B * yBuffer[channel][1] - params.D * uBuffer[channel][0] - params.E * uBuffer[channel][1]) / params.C;
            }

            if (yUpdate > this->cutOffVoltage) {
                yUpdate = this->cutOffVoltage;
                tmpU = (this->cutOffVoltage - updatedParams.A * yUBuffer[channel][0] - updatedParams.B * yUBuffer[channel][1] - updatedParams.D * uUBuffer[channel][0] - updatedParams.E * uUBuffer[channel][1]) / updatedParams.C;
            }
            else if (yUpdate < -this->cutOffVoltage) {
                yUpdate = -this->cutOffVoltage;
                tmpU = (-this->cutOffVoltage - updatedParams.A * yUBuffer[channel][0] - updatedParams.B * yUBuffer[channel][1] - updatedParams.D * uUBuffer[channel][0] - updatedParams.E * uUBuffer[channel][1]) / updatedParams.C;
            }


            this->yBuffer[channel].push(yCurrent);
            this->uBuffer[channel].push(tmp);
            this->yUBuffer[channel].push(yUpdate);
            this->uUBuffer[channel].push(tmpU);


        


            //RC
            tmp = yCurrent;
            tmpU = yUpdate;
            yCurrent = params.F * yRCBuffer[channel] + params.G * yCurrent + params.H * uRCBuffer[channel];
            yUpdate = updatedParams.F * yURCBuffer[channel] + updatedParams.G * yUpdate + updatedParams.H * uURCBuffer[channel];

            this->yRCBuffer[channel] = yCurrent;
            this->uRCBuffer[channel] = tmp;
            this->yURCBuffer[channel] = yUpdate;
            this->uURCBuffer[channel] = tmpU;

            float mult = this->crossfade.getCurrentValue();
            channelSamples[i] = mult * yCurrent + (1.0f - mult) * yUpdate;
            this->crossfade.getNextValue();

            if(std::abs(channelSamples[i])>std::abs(this->maxOutput))
                this->maxOutput = channelSamples[i];
        }
    }
}

void Stage2::configure(double sampleRate) {
    
    this->sampleRate = static_cast<float>(sampleRate) * (this->resampled ? INT_SIZE : 1);
    float T = 1 / this->sampleRate;
    float G2 = *this->gainParameter;
    float G1 = 10000.01f - G2;

    ParameterSet2 newValues;

    float G = 1 / (20291.894071046987289160560623360875 * G1 + 151796712417.358253480438885062842882457600 * T + 30437841.106570480933740840935041312500 + 22300745198530.623141535718272648361505980416 * T * T + 98123278.873534746827226483276371722240 * G1 * T);
    newValues. A = -(-60875682.213140961867481681870082625000 - 40583.788142093974578321121246721750 * G1 + 44601490397061.246283071436545296723011960832 * T * T) * G;
    newValues. B = -(20291.894071046987289160560623360875 * G1 - 151796712417.358253480438885062842882457600 * T + 22300745198530.623141535718272648361505980416 * T * T - 98123278.873534746827226483276371722240 * G1 * T + 30437841.106570480933740840935041312500) * G;
    newValues. C = (452034.321114599908335129935588485875 * G1 + 10062247878644.367685981792747769915087257600 * T + 2707240888.776598591418750965718816312500 + 22300745198530.623141535718272648361505980416 * T * T + 98123278.873534746827226483276371722240 * G1 * T) * G;
    newValues. D = (-5414481777.553197182837501931437632625000 - 904068.642229199816670259871176971750 * G1 + 44601490397061.246283071436545296723011960832 * T * T) * G;
    newValues. E = (452034.321114599908335129935588485875 * G1 - 10062247878644.367685981792747769915087257600 * T + 22300745198530.623141535718272648361505980416 * T * T - 98123278.873534746827226483276371722240 * G1 * T + 2707240888.776598591418750965718816312500) * G;
    
    //RC
    G = 1 / (1065365.878535392067584 * G2 + 236118324143.482260684800000 * T + 23611832.414348226068480 * G2 * T + 10431707560.659046277054464);
    newValues.F = -(236118324143.482260684800000 * T - 1065365.878535392067584 * G2 + 23611832.414348226068480 * G2 * T - 10431707560.659046277054464) * G;
    newValues.G = (1043170.756065904715625 * G2) * G;
    newValues.H = (-1043170.756065904715625 * G2) * G;

    this->updatedParameters.push(newValues);
}

void Stage2::initParameters(std::atomic<float>* gainParameter) {
    this->gainParameter = gainParameter;
    this->crossfade.reset(CROSSFADE_SAMPLES);
    this->maxOutput  = this->maxBetween = 0.0f;
    
}
