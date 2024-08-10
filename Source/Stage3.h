#pragma once
#include "Stage.h"
#include "juce_dsp/juce_dsp.h"
#include <vector>

class Stage3 :public Stage {
private:
	std::array<float, 4> G;
	std::array<float, 16> H;
	std::array<float, 4> J;
	std::array<float, 4> D;
	float E;
	float K;
	std::vector<std::array<float, 4>> w;
	std::vector<float> yBuffer;
	std::vector<float> uBuffer;
	std::vector<float> y;
	std::vector<float> x;
	std::vector<float> p;
	std::atomic<float>* odParameter = nullptr;
	std::atomic<float>* odButton = nullptr;
	float R35;
	juce::CriticalSection processLock;


public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* odParameter, std::atomic<float>* odButton);
};