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
    gainLabel.setText("Gain", juce::dontSendNotification);
    this->addAndMakeVisible(gainLabel);

    this->addAndMakeVisible(gainSlider);
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "gain", gainSlider));
    

    bassLabel.setText("Bass", juce::dontSendNotification);
    this->addAndMakeVisible(bassLabel);

    this->addAndMakeVisible(bassSlider);
    bassAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "bass", bassSlider));


    midLabel.setText("Mid", juce::dontSendNotification);
    this->addAndMakeVisible(midLabel);

    this->addAndMakeVisible(midSlider);
    midAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "mid", midSlider));

    
    trebleLabel.setText("Treble", juce::dontSendNotification);
    this->addAndMakeVisible(trebleLabel);

    this->addAndMakeVisible(trebleSlider);
    trebleAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "treble", trebleSlider));

    
    setSize (400, 300);

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
    this->gainSlider.setBounds(15, 15, 300, 300);
    this->bassSlider.setBounds(15, 50, 300, 300);
    this->midSlider.setBounds(15, 100, 300, 300);
    this->trebleSlider.setBounds(15, 125, 300, 300);
}
