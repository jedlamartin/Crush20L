#pragma once

#include <vector>
#include "JuceHeader.h"
#include "Stage.h"

class Stage2 : public Stage {
private:
	float uk, uk1, uk2, uk3, uk4;
	std::atomic<float>* gainParameter = nullptr;
	std::vector<std::array<float, 4>> yBuffer;
	std::vector<std::array<float, 4>> uBuffer;
public:
	void processBlock(juce::AudioBuffer<float>& buffer) override {};
	void configure(double sampleRate) override;
	void initParameters(juce::AudioProcessorValueTreeState& vts);
};