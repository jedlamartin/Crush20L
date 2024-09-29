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

	std::array<float, 4> GSat;
	std::array<float, 16> HSat;
	std::array<float, 4> JSat;
	std::array<float, 4> DSat;
	float ESat;
	float KSat;



	std::vector<std::array<float, 4>> w;
	std::array<float, 4> wSave;

	std::vector<float> yBuffer;	
	float ySave;

	std::vector<float> uBuffer;
	std::vector<float> y;
	std::vector<float> x;
	std::vector<float> p;
	std::vector<float> pSat;
	std::atomic<float>* odParameter = nullptr;
	std::atomic<float>* odButton = nullptr;
	float R35;
	float cutOffVoltage;
	juce::CriticalSection processLock;
	float wmax;
	float inmax;

public:
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* odParameter, std::atomic<float>* odButton);
};