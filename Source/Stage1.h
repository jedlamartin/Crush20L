#pragma once

#include "Stage.h"
#include "CircularBuffer.hpp"

class Stage1 : public Stage{
private:
	float A, B, C, D;
	std::vector<CircularBuffer<float, 2>> yBuffer;
	std::vector<CircularBuffer<float, 2>> uBuffer;
public:
	Stage1();
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
};