/*
  ==============================================================================

    Stage4.h
    Created: 24 Apr 2024 5:06:36pm
    Author:  Martin

  ==============================================================================
*/

#pragma once
#include "Stage.h"

class Stage4 :public Stage {
private:
	double A, B, C, D, E, F, G, H, I;
	std::atomic<float>* bassParameter = nullptr;
	std::atomic<float>* midParameter = nullptr;
	std::atomic<float>* trebleParameter = nullptr;
	std::vector<std::array<float, 4>> yBuffer;
	std::vector<std::array<float, 4>> uBuffer;
public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(juce::AudioProcessorValueTreeState& vts);

};