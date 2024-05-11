#pragma once
#include "Stage.h"
#include "juce_dsp/juce_dsp.h"
#include <vector>

class Stage3WithMatrix :public Stage {
private:
	juce::dsp::Matrix<float> G;
	juce::dsp::Matrix<float> H;
	juce::dsp::Matrix<float> J;
	juce::dsp::Matrix<float> D;
	juce::dsp::Matrix<float> E;
	juce::dsp::Matrix<float> K;
	std::vector<juce::dsp::Matrix<float>> w;
	std::vector<float> yBuffer;
	std::vector<float> uBuffer;
	std::vector<float> y;
	std::vector<float> x;
	std::vector<float> p;
	std::atomic<float>* odParameter = nullptr;
	std::atomic<float>* odButton = nullptr;
	float R35;

public:
	Stage3WithMatrix();
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* odParameter, std::atomic<float>* odButton);
};