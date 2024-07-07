/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"
#include "Stage6.h"
#include "ReSample.hpp"


//==============================================================================
/**
*/

class ParameterAttachment : public juce::AudioProcessorValueTreeState::Listener {
private:
    Stage* stageToListenTo;
    juce::AudioProcessor* processor;

    void parameterChanged(const juce::String& parameterID, float newValue) override {
        stageToListenTo->configure(processor->getSampleRate());
    }

public:
    ParameterAttachment(Stage* stageToListenTo, juce::AudioProcessor* processor) : stageToListenTo(stageToListenTo), processor(processor) {};


};


class OrangeCrush20LAudioProcessor  : public juce::AudioProcessor{
public:
    //==============================================================================
    OrangeCrush20LAudioProcessor();
    ~OrangeCrush20LAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:




    juce::AudioProcessorValueTreeState parameters;

    ReSample<4,5> resample;

    Stage1 stage1;
    Stage2 stage2;
    Stage3 stage3;
    Stage4 stage4;
    Stage5 stage5;
    Stage6 stage6;
    ParameterAttachment stage2Attachment, stage3Attachment, stage4Attachment, stage5Attachment;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrangeCrush20LAudioProcessor)
};