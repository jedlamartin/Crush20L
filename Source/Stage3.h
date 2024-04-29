#pragma once
#include "Stage.h"
#include "juce_dsp/juce_dsp.h"

class Stage3 :public Stage {
private:
	juce::dsp::Matrix<float> G;
	juce::dsp::Matrix<float> H;
	juce::dsp::Matrix<float> J;
	juce::dsp::Matrix<float> K;
	std::atomic<float>* odParameter = nullptr;
	std::atomic<float>* odButton = nullptr;

public:
	Stage3();
	void processBlock(juce::AudioBuffer<float>& buffer) override {};
	void configure(double sampleRate) override;
	void initOd(juce::AudioProcessorValueTreeState& vts);

};