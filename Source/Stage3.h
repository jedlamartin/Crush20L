#pragma once
#include "Stage.h"
#include "juce_dsp/juce_dsp.h"
#include <vector>

/*struct ParameterSet {
	double G, H, J, D, E, K, G, H, I;
	ParameterSet() :A(0.0), B(0.0), C(0.0), D(0.0), E(0.0), F(0.0), G(0.0), H(0.0), I(0.0) {}
};*/

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
	const float R35;
	const float cutOffVoltage;
	float wMax;
	float maxInput;
	float maxOutput;

public:
	Stage3();
	void processBlock(juce::AudioBuffer<float>& buffer) override;
	void configure(double sampleRate) override;
	void initParameters(std::atomic<float>* odParameter, std::atomic<float>* odButton);
};