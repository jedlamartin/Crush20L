#pragma once

#include "Stage.h"

class Stage1 : public Stage{
private:
	float A, B, C, D;
	std::vector<std::array<float, 2>> yBuffer;
	std::vector<std::array<float, 2>> uBuffer;
public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
};