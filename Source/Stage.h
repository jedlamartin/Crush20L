#pragma once

#include <vector>
#include "JuceHeader.h"

#define TL072_CUTOFF 13.45f
#define CROSSFADE_SAMPLES 14400

class Stage {
protected:
	float sampleRate;
public:
	virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
	virtual void configure(double sampleRate) = 0;
};