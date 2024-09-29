#pragma once

#include <vector>
#include "JuceHeader.h"
#include "Stage.h"

class Stage2 : public Stage {
private:
	float A, B, C, D, E, F, G, H;
	std::atomic<float>* gainParameter = nullptr;
	std::vector<std::array<float, 2>> yBuffer;
	std::vector<std::array<float, 2>> uBuffer;
	std::vector<float> yRCBuffer;
	std::vector<float> uRCBuffer;
	float cutOffVoltage;
	juce::CriticalSection processLock;
	float maxO;

public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(juce::AudioProcessorValueTreeState& vts);
};