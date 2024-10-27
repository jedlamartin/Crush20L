#pragma once

#include "Stage.h"
#include "CircularBuffer.hpp"

class Stage1 : public Stage{
private:
	float A, B, C, D;
	std::vector<CircularBuffer<float, 2>> yBuffer;
	std::vector<CircularBuffer<float, 2>> uBuffer;
	float maxInput, maxOutput;
public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
};