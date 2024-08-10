/*
  ==============================================================================

    Stage6.h
    Created: 11 May 2024 4:45:47pm
    Author:  Martin

  ==============================================================================
*/

#pragma once
#include "Stage.h"
#include <vector>


class Stage6 :public Stage {
private:
    float A, B, C;
    std::vector<float> yBuffer;
    std::vector<float> uBuffer;
    float maxtestInput, maxtestOutput;

public:
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void configure(double sampleRate) override;
};