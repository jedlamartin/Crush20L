#include <JuceHeader.h>
#include "Stage1.h"


Stage1::Stage1():Stage(false){}

void Stage1::processBlock(juce::AudioBuffer<float>& buffer) {
	int channelNumber = buffer.getNumChannels();
	this->yBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});
	this->uBuffer.resize(channelNumber, CircularBuffer<float, 2>{0.0f});

	// y[k]=Ay[k-1]+By[k-2]+Cu[k]+Du[k-2];
	for (int channel = 0; channel < channelNumber; channel++) {
		auto channelSamples=buffer.getWritePointer(channel);
		for (int i = 0; i < buffer.getNumSamples(); i++) {
			float tmp = channelSamples[i];

			channelSamples[i] = this->A * this->yBuffer[channel][0] + this->B * this->yBuffer[channel][1] + this->C * channelSamples[i] + this->D * this->uBuffer[channel][1];
			this->yBuffer[channel].push(channelSamples[i]);
			this->uBuffer[channel].push(tmp);

		}
	}


}

void Stage1::configure(double sampleRate) {
	this->sampleRate = static_cast<float>(sampleRate) * (this->resampled ? INT_SIZE : 1);
	float T = 1 / this->sampleRate;
	float G = static_cast<float>(1 / (5629499.534213120000000 * T * T + 554097.002453808513024 * T + 4.974225788430713));
	this->A = static_cast<float>(-(11258999.068426240000000 * T * T - 9.948451576861426) * G);
	this->B = static_cast<float>(-(5629499.534213120000000 * T * T - 554097.002453808513024 * T + 4.974225788430713) * G);
	this->C = static_cast<float>(529172.956216033345536 * T * G);
	this->D = static_cast<float>(-529172.956216033345536 * T * G);
}


