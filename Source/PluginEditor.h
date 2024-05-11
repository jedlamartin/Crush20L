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

class LookAndFeel :public juce::LookAndFeel_V4 {
public:
    //void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
};

class Knob :public juce::Slider {
private:
    juce::RangedAudioParameter* param; //https://forum.juce.com/t/getrawparametervalue-vs-getparameter/38395
    LookAndFeel lookAndFeel;
public:
    Knob(juce::RangedAudioParameter& param);
    //void paint(juce::Graphics& g) override;

};



class OrangeCrush20LAudioProcessorEditor  : public juce::AudioProcessorEditor {
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

    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    juce::Slider bassSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;


    juce::Slider midSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midAttachment;


    juce::Slider trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;

    juce::Slider odSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> odAttachment;

    juce::ToggleButton odButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> odButtonAttachment;

    juce::Slider volSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrangeCrush20LAudioProcessorEditor)
};
