/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

OrangeCrush20LAudioProcessorEditor::OrangeCrush20LAudioProcessorEditor (OrangeCrush20LAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)

{

    this->addAndMakeVisible(gainSlider);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "gain", gainSlider));

    this->addAndMakeVisible(bassSlider);
    bassAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "bass", bassSlider));

    this->addAndMakeVisible(midSlider);
    midAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "mid", midSlider));

    this->addAndMakeVisible(trebleSlider);
    trebleAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "treble", trebleSlider));

    this->addAndMakeVisible(odSlider);
    odAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "od", odSlider));
    
    this->addAndMakeVisible(odButton);
    odButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "odButton", odButton));
    
    this->addAndMakeVisible(volSlider);
    volAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "vol", volSlider));


    
    setSize (1000, 400);

}

OrangeCrush20LAudioProcessorEditor::~OrangeCrush20LAudioProcessorEditor()
{
}

//==============================================================================
void OrangeCrush20LAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void OrangeCrush20LAudioProcessorEditor::resized()
{
    this->gainSlider.setBounds(15, 0, 300, 50);
    this->bassSlider.setBounds(15, 100, 300, 50);
    this->midSlider.setBounds(15, 200, 300, 50);
    this->trebleSlider.setBounds(15, 300, 300, 50);
    this->odSlider.setBounds(400, 0, 300, 50);
    this->odButton.setBounds(400, 100, 300, 50);
    this->volSlider.setBounds(400, 200, 300, 50);
}

Knob::Knob(juce::RangedAudioParameter& param) :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalDrag, NoTextBox), param(&param) {
    this->setLookAndFeel(&this->lookAndFeel);
}
