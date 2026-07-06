/*
  ==============================================================================

    Cab.h
    Created: 26 Jun 2026 5:44:51pm
    Author:  zero6575

  ==============================================================================
*/

#pragma once

#include <array>
#include <vector>
#include <JuceHeader.h>
#include "CircularBuffer.hpp"
#include "CabHelpers.h"


enum ActiveCab {
    DEFAULT_CAB,
    NO_CAB,
    LOADED_CAB
};

class Cabs {
public:
	Cabs();
    void loadCab();
    void processBlock(juce::AudioBuffer<float>& buffer, ActiveCab ac);
private:
    std::array<float, FIR_TAPS> default_cab;
    std::array<float, FIR_TAPS> loaded_cab;

    std::array<float, FIR_TAPS> ir_loading_buffer;
    std::atomic<bool> new_cab_ready{ false };

    std::vector<CircularBuffer<float, FIR_TAPS>> cab_buf;
	ActiveCab activeCab;
};