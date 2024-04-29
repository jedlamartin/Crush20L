/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OrangeCrush20LAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OrangeCrush20LAudioProcessorEditor(OrangeCrush20LAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~OrangeCrush20LAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OrangeCrush20LAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Label gainLabel;
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    juce::Label bassLabel;
    juce::Slider bassSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;


    juce::Label midLabel;
    juce::Slider midSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midAttachment;


    juce::Label trebleLabel;
    juce::Slider trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrangeCrush20LAudioProcessorEditor)
};
