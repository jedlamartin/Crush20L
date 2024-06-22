/*
  ==============================================================================

    ReSample.h
    Created: 14 May 2024 10:45:12pm
    Author:  Martin

  ==============================================================================
*/

#pragma once

#include <vector>
#include "CircularBuffer.hpp"
#include <JuceHeader.h>
#include <cmath>

template <size_t N>

class ReSample {
private:
    std::array<float, (N * 2 + 1) * 4> sinc;
    std::vector<CircularBuffer<float, N>> IPBuffer;
    std::vector<CircularBuffer<float, N * 4>> DMBuffer;
    juce::AudioBuffer<float> IPAudioBuffer;

public:
    ReSample() :sinc{ 0 } {};
    juce::AudioBuffer<float>& getBuffer() {
        return this->IPAudioBuffer;
    }
    void configure(double sampleRate) {
        float T = static_cast <float>(1 / sampleRate);
        float fs = static_cast<float> (sampleRate);
        int i = 0;
        float n = -static_cast<float>(N);
        while (i < N * 2 + 1) {
            sinc[i * 4] = std::sin(juce::MathConstants<float>::pi * fs * T * n) / (juce::MathConstants<float>::pi * fs * T * n);
            sinc[i * 4 + 1] = std::sin(juce::MathConstants<float>::pi * fs * T * (n + 1.0f / 4.0f)) / (juce::MathConstants<float>::pi * fs * T * (n + 1.0f / 4.0f));
            sinc[i * 4 + 2] = std::sin(juce::MathConstants<float>::pi * fs * T * (n + 1.0f / 2.0f)) / (juce::MathConstants<float>::pi * fs * T * (n + 1.0f / 2.0f));
            sinc[i * 4 + 3] = std::sin(juce::MathConstants<float>::pi * fs * T * (n + 3.0f / 4.0f)) / (juce::MathConstants<float>::pi * fs * T * (n + 3.0f / 4.0f));
            i++;
            n++;
        }
        sinc[4 * N] = 1;
    }
    void interpolate(const juce::AudioBuffer<float>& buffer) {
        int intN = static_cast<int>(N);
        int channelNumber = buffer.getNumChannels();
        int sampleNumber = buffer.getNumSamples();
        this->IPAudioBuffer.setSize(channelNumber, sampleNumber * 4, false, true, true);
        this->IPBuffer.resize(channelNumber, CircularBuffer<float, N>(0));


        for (int channel = 0; channel < channelNumber; channel++) {
            const float* from = buffer.getReadPointer(channel);
            float* to = this->IPAudioBuffer.getWritePointer(channel);

            for (int l = 0; l < sampleNumber * 4; l++) {
                int delta = l % 4;
                if (delta != 0) {
                    to[l] = 0;
                    int fromI = juce::jmax<int>(-intN, l / 4 - sampleNumber);
                    for (int n = fromI; n <= 0; n++) {
                        to[l] += sinc[(n + intN) * 4 + delta] * from[l / 4 - n];
                    }

                    for (int n = 1; n <= intN; n++) {
                        to[l] += sinc[(n + intN) * 4 + delta] * IPBuffer[channel][n - 1];
                    }

                }
                else {
                    to[l] = from[l / 4];
                    this->IPBuffer[channel].push(from[l / 4]);
                }
            }
        }

    }
    void decimate(juce::AudioBuffer<float>& buffer) {
        int intN = static_cast<int>(N);
        int channelNumber = buffer.getNumChannels();
        int sampleNumber = buffer.getNumSamples();
        this->DMBuffer.resize(channelNumber, CircularBuffer<float, N * 4>(0));


        for (int channel = 0; channel < channelNumber; channel++) {
            const float* from = this->IPAudioBuffer.getReadPointer(channel);
            float* to = buffer.getWritePointer(channel);
            for (int l = 0; l < sampleNumber; l++) {
                to[l] = 0;

                int fromI = juce::jmax<int>(-intN * 4, l - sampleNumber);
                for (int n = fromI; n <= 0; n++) {
                    to[l] += sinc[n + 4 * intN] * from[l * 4 - n];
                }

                for (int n = 1; n <= intN * 4; n++) {
                    to[l] += sinc[n + 4 * intN] * DMBuffer[channel][n - 1];
                }

                for (int i = 1; i <= intN; i++) {
                    DMBuffer[channel].push(from[l * 4 + i]);
                }
            }
        }
    }

};


