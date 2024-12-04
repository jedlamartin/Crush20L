#pragma once

#include <vector>
#include "CircularBuffer.hpp"
#include <JuceHeader.h>
#include <cmath>




template <int iSize, int N>
class kaiserArray {
private:
    std::array<float, (N + 1)* iSize> kaiser;
    const float beta;
    const float M;
public:
    kaiserArray(float beta) :kaiser{ 0 }, beta(beta), M((N + 1)* iSize*2) {
        for (int n = 0; n < (N + 1) * iSize; n++) {
            float arg = beta * std::sqrt(1 - (2.0f * n / M) * (2.0f * n / M));
            float num = besselI0(arg);
            float den = besselI0(beta);
            kaiser[n] = num / den;
        }
    };
    float& operator[](int i) {
        return kaiser[i < 0 ? -i : i];
    }

    size_t size() const {
        return this->kaiser.size();
    }

    float besselI0(float x) {
        const float epsilon = 1e-4f; // Pontossági határ
        float sum = 1.0f;           // Sorösszeg
        float term = 1.0f;          // Egyes tagok
        float k = 1.0f;             // Iterációs változó
        float factorial = 1.0f;     // Faktoriális kiszámítása

        while (term > epsilon * sum) {
            factorial *= k; // k! kiszámítása
            term = std::pow(x / 2.0f, 2 * k) / (factorial * factorial); // Jelenlegi tag
            sum += term;
            k += 1.0f;
        }

        return sum;
    }
};

template <int iSize, int N>
class sincArray {
private:
    std::array<float, (N + 1) * iSize> sinc;
public:
    sincArray() :sinc{ 0 } {}

    float& operator[](int i) {
        return sinc[i < 0 ? -i : i];
    }

    float& operator()(int i, int delta) {
        if (i < 0)
            return (*this)[(-i) * iSize - delta];
        return (*this)[i * iSize + delta];

    }

    void configure(float sampleRate) {
        float fc = sampleRate / 2;
        float T = 1 / sampleRate;
        for (int i = 0; i <= N; i++) {
            for (int delta = 0; delta < iSize; delta++) {
                float index = static_cast<float>(i) + (1.0f / static_cast<float>(iSize)) * static_cast<float>(delta);
                (*this)(i, delta) = std::sin(2.0f * juce::MathConstants<float>::pi * fc * index * T) / (2.0f * juce::MathConstants<float>::pi * fc * index * T);
            }
        }
        (*this)[0] = 1;
    }

    size_t size() const {
        return this->sinc.size();
    }

    void applyKaiser(float beta) {
        kaiserArray<iSize, N> kaiser(beta);
        for (int i = 0; i < sinc.size(); i++) {
            sinc[i] = sinc[i] * kaiser[i];
        }
    }
};

template <int iSize, int N>

class ReSample {
private:
    sincArray<iSize, N> sinc;
    juce::AudioBuffer<float> interpolatedBuf;
    std::vector<CircularBuffer<float, N>> beginBuf, endBuf;
    std::vector<CircularBuffer<float, N * iSize>> decBeginBuf, decEndBuf;

public:
    void configure(double sampleRate){
        sinc.configure(static_cast<float>(sampleRate));
        sinc.applyKaiser(5.0f);
        beginBuf.clear();
        endBuf.clear();
        decBeginBuf.clear();
        decEndBuf.clear();
    }
    void interpolate(juce::AudioBuffer<float>& buffer) {
        int channelSize = buffer.getNumChannels();
        int originalBufSize = buffer.getNumSamples();
        int interpolatedBufSize = originalBufSize * iSize;
        beginBuf.resize(channelSize, CircularBuffer<float, N>(0.0f));
        endBuf.resize(channelSize, CircularBuffer<float, N>(0.0f));
        juce::AudioBuffer<float> x(channelSize, originalBufSize + N);
        for (int channel = 0; channel < channelSize; channel++) {
            x.copyFrom(channel, N, buffer, channel, 0, originalBufSize);
            float* currentSample = x.getWritePointer(channel);
            for (int i = 0; i < N; i++) {
                //forditva masolom bele
                currentSample[N-i-1] = endBuf[channel][i];
            }
        }


        interpolatedBuf.setSize(channelSize, interpolatedBufSize, true, true, true);
        for (int channel = 0; channel < channelSize; channel++) {
            float const* channelSamples = x.getReadPointer(channel);
            float* iSamples = interpolatedBuf.getWritePointer(channel);
            //bevart mintak
            //az elsot automatikusan kitoltjuk, hogy a buffereles jo legyen
            *iSamples = *channelSamples;
            for (int k = 1; k < interpolatedBufSize; k++) {
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
            beginBuf[channel].push(channelSamples[originalBufSize - 1]);
            //endbuf feltoltese ha kesz minden
            for (int i = 0; i < N; i++) {
                endBuf[channel].push(channelSamples[originalBufSize + i]);
            }
        }


    }
    void decimate(juce::AudioBuffer<float>& buffer) {
        int channelSize = buffer.getNumChannels();
        int originalBufSize = buffer.getNumSamples();
        int interpolatedBufSize = this->interpolatedBuf.getNumSamples();
        decBeginBuf.resize(channelSize, CircularBuffer<float, N * iSize>(0.0f));
        decEndBuf.resize(channelSize, CircularBuffer<float, N * iSize>(0.0f));

        //bepakolni az elozo veget az elejere
        juce::AudioBuffer<float> x(channelSize, interpolatedBufSize + N * iSize);
        for (int channel = 0; channel < channelSize; channel++) {
            x.copyFrom(channel, N * iSize, interpolatedBuf, channel, 0, interpolatedBufSize);
            float* currentSample = x.getWritePointer(channel);
            for (int i = 0; i < N * iSize; i++) {
                currentSample[N * iSize - i - 1] = decEndBuf[channel][i];
            }
        }


        for (int channel = 0; channel < channelSize; channel++) {
            float const* iSamples = x.getReadPointer(channel);
            float* samples = buffer.getWritePointer(channel);
            for (int k = 0; k < originalBufSize; k++) {
                int index = iSize * k;
                samples[k] = 0;

                //bufferbol
                for (int n = 1; n <= N * iSize; n++) { 
                    samples[k] += sinc[n] * decBeginBuf[channel][n - 1];
                }


                //mintakbol 
                for (int n = 0; n >= -(N*iSize); n--) {
                    samples[k] += sinc[n] * iSamples[index - n];
                }

                //az elemek visszapusholasa 0-3
                for (int i = 0; i < iSize; i++) {
                    decBeginBuf[channel].push(iSamples[index + i]);
                }



                samples[k] /= iSize;

            }

            //endbuf feltoltese ha kesz minden
            for (int i = 0; i < N * iSize; i++) {
                decEndBuf[channel].push(iSamples[interpolatedBufSize + i]);
            }
        }
    }

    void process(std::function<void(juce::AudioBuffer<float>&)> processBlock) {
        processBlock(this->interpolatedBuf);
    }
    
};


