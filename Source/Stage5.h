#pragma once
#include "Stage.h"
#include <vector>
#include "ParameterQueue.hpp"
#include "CircularBuffer.hpp"

struct ParameterSet5 {
    float A, B, C, D, E, F, G, H, I, J, K, L, M;
    ParameterSet5() :A(0.0f), B(0.0f), C(0.0f), D(0.0f), E(0.0f), F(0.0f), G(0.0f), H(0.0f), I(0.0f), J(0.0f), K(0.0f), L(0.0f), M(0.0f) {}
};

class Stage5 :public Stage {
private:

    ParameterSet5 params;
    ParameterSet5 updatedParams;
    ParameterQueue<ParameterSet5> updatedParameters;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> crossfade{ 1.0f };


    std::vector<CircularBuffer<float, 2>> yBuffer;
    std::vector<CircularBuffer<float, 2>> uBuffer;
    std::vector<float> yNegBuffer;
    std::vector<float> uNegBuffer;
    std::vector<CircularBuffer<float, 2>> yVolBuffer;
    std::vector<CircularBuffer<float, 2>> uVolBuffer;
    std::vector<CircularBuffer<float, 2>> yUBuffer;
    std::vector<CircularBuffer<float, 2>> uUBuffer;
    std::vector<float> yUNegBuffer;
    std::vector<float> uUNegBuffer;
    std::vector<CircularBuffer<float, 2>> yUVolBuffer;
    std::vector<CircularBuffer<float, 2>> uUVolBuffer;

    std::atomic<float>* volParameter = nullptr;


public:
    Stage5();
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void configure(double sampleRate) override;
    void initParameters(std::atomic<float>* volParameter);

};