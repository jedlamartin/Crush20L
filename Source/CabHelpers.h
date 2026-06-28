#pragma once
#include <JuceHeader.h>

constexpr size_t FIR_TAPS = 1024;

void loadWavFromMemory(const void* cab_wav, size_t cab_wavSize, std::array<float, FIR_TAPS>& cab_ir);

void loadWavFromFileChooser(std::array<float, FIR_TAPS>& cab_ir, std::atomic<bool>& readyFlag);

