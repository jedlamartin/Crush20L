#include "Cab.h"

Cabs::Cabs() :default_cab{}, loaded_cab{}, ir_loading_buffer{}, activeCab{ActiveCab::DEFAULT_CAB}, cab_buf{} {
	loadWavFromMemory(BinaryData::orange_cab_wav, static_cast<size_t>(BinaryData::orange_cab_wavSize), default_cab);
}

void Cabs::loadCab(){
    loadWavFromFileChooser(loaded_cab, new_cab_ready);
}

void Cabs::processBlock(juce::AudioBuffer<float>& buffer, ActiveCab ac){
    if (new_cab_ready.load()) {
        std::copy(ir_loading_buffer.begin(), ir_loading_buffer.end(), loaded_cab.begin());
        new_cab_ready.store(false);

        if (ac == ActiveCab::LOADED_CAB) {
            for (auto& buf : cab_buf) {
                buf.clear();
            }
        }
    }

    if (activeCab != ac) {
        if (ac != ActiveCab::LOADED_CAB) {
            for (auto& buf : cab_buf) {
                buf.clear();
            }
        }

        activeCab = ac;
    }

    const std::array<float, FIR_TAPS>* activeCabArray = nullptr;
    if (ac == DEFAULT_CAB)       activeCabArray = &default_cab;
    else if (ac == LOADED_CAB)   activeCabArray = &loaded_cab;
    else return;

    const auto& cabIr = *activeCabArray;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelSamples = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            cab_buf[channel].push(channelSamples[sample]);

            float outputSample = 0.0f;
            for (size_t tap = 0; tap < FIR_TAPS; ++tap) {
                outputSample += cab_buf[channel][tap] * cabIr[tap];
            }
            channelSamples[sample] = outputSample;
        }
    }
}
