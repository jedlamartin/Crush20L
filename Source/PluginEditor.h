/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Switch.h"

//==============================================================================
/**
*/

class MyLookAndFeel :public juce::LookAndFeel_V4 {
private:
    juce::Image knobImage;
    int knobFrames;
    int knobSize;
public:
    MyLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
    int getSliderPopupPlacement(juce::Slider&) override;
};

class Knob :public juce::Slider {
private:
    //juce::RangedAudioParameter* param; //https://forum.juce.com/t/getrawparametervalue-vs-getparameter/38395
    MyLookAndFeel lookAndFeel;
public:
    Knob();
    void paint(juce::Graphics& g) override;
    juce::String getTextFromValue(double value) override;
    ~Knob();
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrangeCrush20LAudioProcessorEditor)
};
