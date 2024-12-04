#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"



class OrangeCrush20LAudioProcessorEditor  : public juce::AudioProcessorEditor {
public:
    OrangeCrush20LAudioProcessorEditor(OrangeCrush20LAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~OrangeCrush20LAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OrangeCrush20LAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Image background;

    Knob gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    Knob bassSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;


    Knob midSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midAttachment;


    Knob trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;

    Knob odSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> odAttachment;

    ToggleSwitch odButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> odButtonAttachment;

    Knob volSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttachment;

    ToggleSwitch powerButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerButtonAttachment;

    ListenerSwitch powerLed;

    Label inputGainLabel;
    VerticalSlider inputGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;


    Label outputGainLabel;
    VerticalSlider outputGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrangeCrush20LAudioProcessorEditor)
};
