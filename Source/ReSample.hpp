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

template <int iSize, int N>
class sincArray {
private:
    std::array<float, (N + 1) * iSize> sinc;
public:
    sincArray() :sinc{ 0 } {}

    float& operator[](int i) {
        //int index = i < 0 ? -i + 1 : i;
        return sinc[i < 0 ? -i - 1 : i];
    }

    float& operator()(int i, int delta) {
        if (i < 0)
            return (*this)[(-i) * iSize - delta];;
        return (*this)[i * iSize + delta];

    }

    void configure(float sampleRate) {
        float fc = sampleRate / 2;
        float T = 1 / sampleRate;
        for (int i = 0; i <= N; i++) {
            for (int delta = 0; delta < iSize; delta++) {
                float index = static_cast<float>(i) + 1 / static_cast<float>(iSize) * static_cast<float>(delta);
                (*this)(i, delta) = std::sin(2.0f * juce::MathConstants<float>::pi * fc * index * T) / (2.0f * juce::MathConstants<float>::pi * fc * index * T);
            }
        }
        (*this)[0] = 1;
    }

    size_t size() const {
        return this->sinc.size();
    }
};

template <size_t iSize, size_t N>

class ReSample {
private:
    sincArray<iSize, N> sinc;
    juce::AudioBuffer<float> interpolatedBuf;
    std::vector<CircularBuffer<float, N>> beginBuf, endBuf;
    std::vector<CircularBuffer<float, N>> decBeginBuf, decEndBuf;

public:
    void configure(double sampleRate){
        sinc.configure(static_cast<float>(sampleRate));
    }
    void interpolate(juce::AudioBuffer<float>& buffer) {
        int channelSize = buffer.getNumChannels();
        int oBufSize = buffer.getNumSamples();
        int iBufSize = oBufSize * iSize;
        beginBuf.resize(channelSize, CircularBuffer<float, N>(0));
        endBuf.resize(channelSize, CircularBuffer<float, N>(0));
        juce::AudioBuffer<float> x(channelSize, buffer.getNumSamples() + N);
        for (int channel = 0; channel < channelSize; channel++) {
            x.copyFrom(channel, N, buffer, channel, 0, buffer.getNumSamples());
            float* currentSample = x.getWritePointer(channel);
            for (int i = 0; i < N; i++) {
                currentSample[i] = endBuf[channel][i];
            }
        }

        /*DBG("\nX=");
        for (int i = 0; i < x.getNumSamples(); i++) {
            float* sample = x.getWritePointer(0);
            DBG(sample[i]);
        }*/

        //eddig megvan
        interpolatedBuf.setSize(channelSize, iBufSize, false, false, false);
        for (int channel = 0; channel < channelSize; channel++) {
            float const* channelSamples = x.getReadPointer(channel);
            float* iSamples = interpolatedBuf.getWritePointer(channel);
            //bevart mintak
            //az elsot automatikusan kitoltjuk, hogy a buffereles jo legyen
            *iSamples = *channelSamples;
            for (int k = 1; k < iBufSize; k++) {
                int delta = k % 4;
                int chI = k / 4;
                if (delta != 0) {
                    iSamples[k] = 0;
                    //mintakbol
                    for (int n = -N; n <= 0; n++) {
                        iSamples[k] += this->sinc(n, delta) * channelSamples[chI-n];
                    }
                    //bufferbol
                    for (int n = 1; n <= N; n++) {
                        iSamples[k] += this->sinc(n, delta) * beginBuf[channel][n - 1];
                    }
                }
                else {
                    iSamples[k] = channelSamples[chI];
                    beginBuf[channel].push(channelSamples[chI-1]);

                }
            }
            //endbuf feltoltese ha kesz minden
            for (int i = 0; i < N; i++) {
                //DBG(x.getNumSamples());
                //DBG(buffer.getNumSamples());
                //DBG(channelSamples[channelSize - 1 + i]);
                endBuf[channel].push(channelSamples[oBufSize - 1 + i]);
            }
            //DBG("\n\n");
        }


    }
    void decimate(juce::AudioBuffer<float>& buffer) {
        int channelSize = buffer.getNumChannels();
        int oBufSize = buffer.getNumSamples();
        int iBufSize = this->interpolatedBuf.getNumSamples();
        decBeginBuf.resize(channelSize, CircularBuffer<float, N>(0));

        for (int channel = 0; channel < channelSize; channel++) {
            float const* iSamples = interpolatedBuf.getReadPointer(channel);
            float* samples = buffer.getWritePointer(channel);
            for (int k = 0; k < oBufSize; k++) {
                samples[k] = 0;
                int index = 4 * k; //hogy a faszba szerezzek még annyi mintát amennyi kell? elvileg már megszereztem, de hogy kezeljem ezt
                for (int n = -N; n <= 0; n++) {
                    samples[k] += sinc[n] * iSamples[index - n];
                }
            }
        }
    }
    
};


