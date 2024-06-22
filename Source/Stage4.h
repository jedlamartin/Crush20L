/*
  ==============================================================================

    Stage4.h
    Created: 24 Apr 2024 5:06:36pm
    Author:  Martin

  ==============================================================================
*/

#pragma once
#include "Stage.h"
#include "CircularBuffer.hpp"

class Stage4 :public Stage {
private:
	double A, B, C, D, E, F, G, H, I;
	//juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> A, B, C, D, E, F, G, H, I;
	std::atomic<float>* bassParameter = nullptr;
	std::atomic<float>* midParameter = nullptr;
	std::atomic<float>* trebleParameter = nullptr;
	std::vector<CircularBuffer<float, 4>> yBuffer;
	std::vector<CircularBuffer<float, 4>> uBuffer;
public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* bassParameter, std::atomic<float>* midParameter, std::atomic<float>* trebleParameter);
	void resetParameters(double sampleRate);
};