/*
  ==============================================================================

    Stage2.cpp
    Created: 19 Apr 2024 8:45:49pm
    Author:  Martin

  ==============================================================================
*/

#include "Stage2.h"

void Stage2::processBlock(juce::AudioBuffer<float>& buffer)
{
    const juce::ScopedLock lock(this->processLock);

    int channelNumber = buffer.getNumChannels();
    this->yBuffer.resize(channelNumber, std::array<float, 2>{0, 0});
    this->uBuffer.resize(channelNumber, std::array<float, 2>{0, 0});
    this->yRCBuffer.resize(channelNumber, 0.0f);
    this->uRCBuffer.resize(channelNumber, 0.0f);

    for (auto channel = 0; channel < channelNumber; channel++) {
        float* channelSamples = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float tmp = channelSamples[i];
            channelSamples[i] = A * yBuffer[channel][0] + B * yBuffer[channel][1] + C * channelSamples[i] + D * uBuffer[channel][0] + E * uBuffer[channel][1];
            
            //Cutoff
            if (channelSamples[i] > this->cutOffVoltage) {
                channelSamples[i] = this->cutOffVoltage;
                tmp = (this->cutOffVoltage - A * yBuffer[channel][0] - B * yBuffer[channel][1] - D * uBuffer[channel][0] - E * uBuffer[channel][1]) / C;
            }
            else if (channelSamples[i] < -this->cutOffVoltage) {
                channelSamples[i] = -this->cutOffVoltage;
                tmp = (-this->cutOffVoltage - A * yBuffer[channel][0] - B * yBuffer[channel][1] - D * uBuffer[channel][0] - E * uBuffer[channel][1]) / C;
            }

            this->yBuffer[channel][1] = this->yBuffer[channel][0];
            this->yBuffer[channel][0] = channelSamples[i];

            this->uBuffer[channel][1] = this->uBuffer[channel][0];
            this->uBuffer[channel][0] = tmp;
        


            //RC
            tmp = channelSamples[i];
            channelSamples[i] = F * yRCBuffer[channel] + G * channelSamples[i] + H * uRCBuffer[channel];
            this->yRCBuffer[channel] = channelSamples[i];
            this->uRCBuffer[channel] = tmp;

        }
    }
}

void Stage2::configure(double sampleRate) {
    const juce::ScopedLock lock(this->processLock);

    float T = 1 / sampleRate;
    float G2 = *this->gainParameter;
    float G1 = 10000.01f - G2;

    float G = 1 / (20291.894071046987289160560623360875 * G1 + 151796712417.358253480438885062842882457600 * T + 30437841.106570480933740840935041312500 + 22300745198530.623141535718272648361505980416 * T * T + 98123278.873534746827226483276371722240 * G1 * T);
    this-> A = -(-60875682.213140961867481681870082625000 - 40583.788142093974578321121246721750 * G1 + 44601490397061.246283071436545296723011960832 * T * T) * G;
    this-> B = -(20291.894071046987289160560623360875 * G1 - 151796712417.358253480438885062842882457600 * T + 22300745198530.623141535718272648361505980416 * T * T - 98123278.873534746827226483276371722240 * G1 * T + 30437841.106570480933740840935041312500) * G;
    this-> C = (452034.321114599908335129935588485875 * G1 + 10062247878644.367685981792747769915087257600 * T + 2707240888.776598591418750965718816312500 + 22300745198530.623141535718272648361505980416 * T * T + 98123278.873534746827226483276371722240 * G1 * T) * G;
    this-> D = (-5414481777.553197182837501931437632625000 - 904068.642229199816670259871176971750 * G1 + 44601490397061.246283071436545296723011960832 * T * T) * G;
    this-> E = (452034.321114599908335129935588485875 * G1 - 10062247878644.367685981792747769915087257600 * T + 22300745198530.623141535718272648361505980416 * T * T - 98123278.873534746827226483276371722240 * G1 * T + 2707240888.776598591418750965718816312500) * G;
    
    //RC
    G = 1 / (1065365.878535392067584 * G2 + 236118324143.482260684800000 * T + 23611832.414348226068480 * G2 * T + 10431707560.659046277054464);
    this->F = -(236118324143.482260684800000 * T - 1065365.878535392067584 * G2 + 23611832.414348226068480 * G2 * T - 10431707560.659046277054464) * G;
    this->G = (1043170.756065904715625 * G2) * G;
    this->H = (-1043170.756065904715625 * G2) * G;

    this->uBuffer.clear();
    this->yBuffer.clear();

    this->uRCBuffer.clear();
    this->yRCBuffer.clear();
}

void Stage2::initParameters(juce::AudioProcessorValueTreeState& vts) {
    this->gainParameter = vts.getRawParameterValue("gain");
    this->cutOffVoltage = 13.45f;
}
