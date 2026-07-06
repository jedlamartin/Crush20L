#pragma once

#include <vector>

#include "JuceHeader.h"


constexpr float TL072_CUTOFF = 13.45f;
constexpr size_t CROSSFADE_SAMPLES = 14400;
constexpr size_t INT_SIZE = 4;
constexpr size_t INT_POINTS = 40;

class Stage {
protected:
    float sampleRate;
    const bool resampled;

public:
    Stage(bool resampled);
    virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
    virtual void configure(double sampleRate) = 0;
};