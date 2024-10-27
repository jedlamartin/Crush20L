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
        return sinc[i < 0 ? -i : i];
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
    std::vector<CircularBuffer<float, N * iSize>> decBeginBuf, decEndBuf;

public:
    void configure(double sampleRate){
        sinc.configure(static_cast<float>(sampleRate));
        beginBuf.clear();
        endBuf.clear();
        decBeginBuf.clear();
        decEndBuf.clear();
    }
    void interpolate(juce::AudioBuffer<float>& buffer) {
        int channelSize = buffer.getNumChannels();
        int oBufSize = buffer.getNumSamples();
        int iBufSize = oBufSize * iSize;
        beginBuf.resize(channelSize, CircularBuffer<float, N>(0));
        endBuf.resize(channelSize, CircularBuffer<float, N>(0));
        juce::AudioBuffer<float> x(channelSize, oBufSize + N);
        for (int channel = 0; channel < channelSize; channel++) {
            x.copyFrom(channel, N, buffer, channel, 0, oBufSize);
            float* currentSample = x.getWritePointer(channel);
            for (int i = 0; i < N; i++) {
                //forditva masolom bele
                currentSample[N-i-1] = endBuf[channel][i];
            }
        }


        interpolatedBuf.setSize(channelSize, iBufSize, true, true, true);
        for (int channel = 0; channel < channelSize; channel++) {
            float const* channelSamples = x.getReadPointer(channel);
            float* iSamples = interpolatedBuf.getWritePointer(channel);
            //bevart mintak
            //az elsot automatikusan kitoltjuk, hogy a buffereles jo legyen
            *iSamples = *channelSamples;
            for (int k = 1; k < iBufSize; k++) {
                int delta = k % iSize;
                int index = k / iSize;
                if (delta != 0) {
                    iSamples[k] = 0;
                    //mintakbol
                    for (int n = -N; n <= 0; n++) {
                        iSamples[k] += this->sinc(n, delta) * channelSamples[index-n];
                    }
                    //bufferbol
                    for (int n = 1; n <= N; n++) {
                        iSamples[k] += this->sinc(n, delta) * beginBuf[channel][n - 1];
                    }
                }
                else {
                    iSamples[k] = channelSamples[index];
                    beginBuf[channel].push(channelSamples[index-1]);

                }
            }
            // az utolso is belekeruljon a bufferbe
            beginBuf[channel].push(channelSamples[oBufSize - 1]);
            //endbuf feltoltese ha kesz minden
            for (int i = 0; i < N; i++) {
                //DBG(x.getNumSamples());
                //DBG(buffer.getNumSamples());
                //DBG(channelSamples[channelSize - 1 + i]);
                endBuf[channel].push(channelSamples[oBufSize + i]);
            }
            //DBG("\n\n");
        }


    }
    void decimate(juce::AudioBuffer<float>& buffer) {
        int channelSize = buffer.getNumChannels();
        int oBufSize = buffer.getNumSamples();
        int iBufSize = this->interpolatedBuf.getNumSamples();
        decBeginBuf.resize(channelSize, CircularBuffer<float, N * iSize>(0));
        decEndBuf.resize(channelSize, CircularBuffer<float, N * iSize>(0));

        //bepakolni az elozo veget az elejere
        juce::AudioBuffer<float> x(channelSize, iBufSize + N * iSize);
        for (int channel = 0; channel < channelSize; channel++) {
            x.copyFrom(channel, N * iSize, interpolatedBuf, channel, 0, iBufSize);
            float* currentSample = x.getWritePointer(channel);
            for (int i = 0; i < N * iSize; i++) {
                currentSample[N * iSize - i - 1] = decEndBuf[channel][i];
            }
        }


        for (int channel = 0; channel < channelSize; channel++) {
            float const* iSamples = x.getReadPointer(channel);
            float* samples = buffer.getWritePointer(channel);
            for (int k = 0; k < oBufSize; k++) {
                samples[k] = 0;
                int index = iSize * k; 

                //bufferbol
                for (int n = 1; n < N * iSize; n++) {
                    samples[k] += sinc[n] * decBeginBuf[channel][n - 1];
                }

                //mintakbol
                for (int n = -1; n > -(N*iSize); n--) {
                    samples[k] += sinc[n] * iSamples[index - n];
                    decBeginBuf[channel].push(iSamples[index - n]);
                }



                samples[k] += sinc[0] * iSamples[index];
                samples[k] /= iSize;

            }

            //endbuf feltoltese ha kesz minden
            for (int i = 0; i < N * iSize; i++) {
                decEndBuf[channel].push(iSamples[iBufSize + i]);
            }
        }
    }

    void test(juce::AudioBuffer<float>& to) {
        for (int channel = 0; channel < to.getNumChannels(); channel++) {
            float const* fromPtr = interpolatedBuf.getReadPointer(channel);
            float* toPtr = to.getWritePointer(channel);
            for (int i = 0; i < to.getNumSamples(); i++) {
                toPtr[i] = fromPtr[i * 4];
            }
            /*DBG(fromPtr[20]);
            DBG(toPtr[5]);
            DBG(" . ");*/

        }
    }

    void process(std::function<void(juce::AudioBuffer<float>&)> processBlock) {
        processBlock(this->interpolatedBuf);
    }
    
};


