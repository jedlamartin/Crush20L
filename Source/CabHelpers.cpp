/*
  ==============================================================================

    CabHelpers.cpp
    Created: 26 Jun 2026 6:19:22pm
    Author:  zero6575

  ==============================================================================
*/

#include "CabHelpers.h"

void loadWavFromMemory(const void* cab_wav, size_t cab_wavSize, std::array<float, FIR_TAPS>& cab_ir) {
    auto inputStream = std::make_unique<juce::MemoryInputStream>(cab_wav, cab_wavSize, false);

	juce::WavAudioFormat wavFormat;
	std::unique_ptr<juce::AudioFormatReader> reader(wavFormat.createReaderFor(inputStream.release(), true));

    if (reader != nullptr) {
        constexpr size_t numChannels = 1;
		juce::AudioBuffer<float> tempBuffer(numChannels, FIR_TAPS);
        reader->read(&tempBuffer, 0, FIR_TAPS, 0, true, false);
        const float* channelData = tempBuffer.getReadPointer(0);
        std::reverse_copy(channelData, channelData + FIR_TAPS, cab_ir.begin());
    }
}

void loadWavFromFileChooser(std::array<float, FIR_TAPS>& cab_ir, std::atomic<bool>& readyFlag) {
    auto chooser = std::make_shared<juce::FileChooser>(
        "Select a Cabinet IR WAV file...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.wav"
    );

    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(folderChooserFlags, [chooser, &cab_ir, &readyFlag](const juce::FileChooser& fc){
		juce::File file = fc.getResult();

        if (file.existsAsFile()) {
            auto inputStream = file.createInputStream();
            if(inputStream != nullptr) {
				juce::WavAudioFormat wavFormat;

				std::unique_ptr<juce::AudioFormatReader> reader(wavFormat.createReaderFor(inputStream.release(), true));
                if (reader != nullptr) {
                    constexpr size_t numChannels = 1;
                    juce::AudioBuffer<float> tempBuffer(numChannels, FIR_TAPS);
                    reader->read(&tempBuffer, 0, FIR_TAPS, 0, true, false);
                    const float* channelData = tempBuffer.getReadPointer(0);
                    std::reverse_copy(channelData, channelData + FIR_TAPS, cab_ir.begin());
                    readyFlag.store(true);
                }
			}
        }
    });
}