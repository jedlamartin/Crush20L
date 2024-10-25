#pragma once

#include <vector>
#include "JuceHeader.h"

#define CROSSFADE_SAMPLES 48000

class Stage {
protected:
	float sampleRate;
public:
	virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
	virtual void configure(double sampleRate) = 0;
};