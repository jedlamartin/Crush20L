#pragma once
#include <vector>

#include "Stage.h"

class Stage6 : public Stage {
private:
    float A, B, C;
    std::vector<float> yBuffer;
    std::vector<float> uBuffer;
    const float cutOffVoltage;

public:
    Stage6();
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void configure(double sampleRate) override;
};