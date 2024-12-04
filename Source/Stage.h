#pragma once

#include <vector>
#include "JuceHeader.h"


#define TL072_CUTOFF 13.45f
#define CROSSFADE_SAMPLES 14400
#define INT_SIZE 4
#define INT_POINTS 40

class Stage {
protected:
	float sampleRate;
	const bool resampled;
public:
	Stage(bool resampled);
	virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
	virtual void configure(double sampleRate) = 0;
};