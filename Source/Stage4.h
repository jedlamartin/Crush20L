/*
  ==============================================================================

    Stage4.h
    Created: 24 Apr 2024 5:06:36pm
    Author:  Martin

  ==============================================================================
*/

#pragma once
#include <queue>
#include "Stage.h"
#include "CircularBuffer.hpp"
#include "ParameterQueue.hpp"


struct ParameterSet4 {
	double A, B, C, D, E, F, G, H, I;
	ParameterSet4() :A(0.0), B(0.0), C(0.0), D(0.0), E(0.0), F(0.0), G(0.0), H(0.0), I(0.0) {}
};

class Stage4 :public Stage {
private:
	//double A, B, C, D, E, F, G, H, I;
	ParameterSet4 params;
	ParameterSet4 updatedParams;
	ParameterQueue<ParameterSet4> updatedParameters;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> crossfade{ 1.0f };

	std::vector<CircularBuffer<float, 4>> yBuffer;
	std::vector<CircularBuffer<float, 4>> uBuffer;
	std::vector<CircularBuffer<float, 4>> yUBuffer;
	std::vector<CircularBuffer<float, 4>> uUBuffer;


	std::atomic<float>* bassParameter = nullptr;
	std::atomic<float>* midParameter = nullptr;
	std::atomic<float>* trebleParameter = nullptr;
public:
	Stage4();
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* bassParameter, std::atomic<float>* midParameter, std::atomic<float>* trebleParameter);
};