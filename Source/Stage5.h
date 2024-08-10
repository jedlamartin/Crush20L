/*
  ==============================================================================

    Stage5.h
    Created: 11 May 2024 12:49:03pm
    Author:  Martin

  ==============================================================================
*/

#pragma once
#include "Stage.h"
#include <vector>

class Stage5 :public Stage {
private:
    float A, B, C, D, E;
    float F, G, H;
    float I, J, K, L, M;
    std::vector<std::array<float, 2>> yBuffer;
    std::vector<std::array<float, 2>> uBuffer;
    std::vector<float> yNegBuffer;
    std::vector<float> uNegBuffer;
    std::vector<std::array<float, 2>> yVolBuffer;
    std::vector<std::array<float, 2>> uVolBuffer;
    std::atomic<float>* volParameter = nullptr;
    juce::CriticalSection processLock;


public:
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void configure(double sampleRate) override;
    void initParameters(std::atomic<float>* volParameter);

};