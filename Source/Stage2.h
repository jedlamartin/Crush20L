#pragma once

#include <vector>
#include "JuceHeader.h"
#include "Stage.h"
#include "ParameterQueue.hpp"
#include "CircularBuffer.hpp"

struct ParameterSet2 {
	double A, B, C, D, E, F, G, H;
	ParameterSet2() :A(0.0), B(0.0), C(0.0), D(0.0), E(0.0), F(0.0), G(0.0), H(0.0) {}
};

class Stage2 : public Stage {
private:
	//float A, B, C, D, E, F, G, H;
	ParameterSet2 params;
	ParameterSet2 updatedParams;
	ParameterQueue<ParameterSet2> updatedParameters;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> crossfade{ 1.0f };


	std::atomic<float>* gainParameter = nullptr;
	std::vector<CircularBuffer<float, 2>> yBuffer;
	std::vector<CircularBuffer<float, 2>> uBuffer;
	std::vector<float> yRCBuffer;
	std::vector<float> uRCBuffer;
	std::vector<CircularBuffer<float, 2>> yUBuffer;
	std::vector<CircularBuffer<float, 2>> uUBuffer;
	std::vector<float> yURCBuffer;
	std::vector<float> uURCBuffer;

	const float cutOffVoltage;
	float maxOutput;
	float maxBetween;

public:
	Stage2();
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* gainParameter);
};