/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

OrangeCrush20LAudioProcessorEditor::OrangeCrush20LAudioProcessorEditor (OrangeCrush20LAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts), 
    powerButton(vts.getRawParameterValue("power"), BinaryData::powerswitch_off_png, BinaryData::powerswitch_off_pngSize, BinaryData::powerswitch_on_png, BinaryData::powerswitch_on_pngSize),
    odButton(vts.getRawParameterValue("odButton"), BinaryData::odbutton_off_png, BinaryData::odbutton_off_pngSize, BinaryData::odbutton_on_png, BinaryData::odbutton_on_pngSize),
    powerLed(vts.getRawParameterValue("power"), BinaryData::led_off_png, BinaryData::led_off_pngSize, BinaryData::led_on_png, BinaryData::led_on_pngSize)

{
    this->background = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);

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
    
        
    this->addAndMakeVisible(volSlider);
    volAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "vol", volSlider));

    this->addAndMakeVisible(powerButton);
    powerButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "power", powerButton));
    powerButton.configure();



    this->addAndMakeVisible(odButton);
    odButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "odButton", odButton));
    odButton.configure();

    this->addAndMakeVisible(powerLed);
    powerButton.pushListener(&powerLed);

    this->addAndMakeVisible(inputGain);
    inputGainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "input", inputGain));
    this->inputGain.setTextValueSuffix(" dB");
    this->addAndMakeVisible(inputGainLabel);
    inputGainLabel.setText("IN", juce::dontSendNotification);
    inputGainLabel.setFont(juce::Font(17.0f));
    inputGainLabel.attachToComponent(&inputGain, true);

    this->addAndMakeVisible(outputGain);
    outputGainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "output", outputGain));
    this->outputGain.setTextValueSuffix(" dB");
    this->addAndMakeVisible(outputGainLabel);
    outputGainLabel.setText("OUT", juce::dontSendNotification);
    outputGainLabel.setFont(juce::Font(17.0f));
    outputGainLabel.attachToComponent(&outputGain, true);

    setSize (1000, 350);

}

OrangeCrush20LAudioProcessorEditor::~OrangeCrush20LAudioProcessorEditor()
{
}

//==============================================================================
void OrangeCrush20LAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    if (this->background.isValid()) {
        g.drawImage(this->background, this->getLocalBounds().toFloat(), juce::RectanglePlacement::onlyReduceInSize | juce::RectanglePlacement::stretchToFit);
    }
    else {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }
}

void OrangeCrush20LAudioProcessorEditor::resized()
{
    this->gainSlider.setBounds(742, 172, 70, 70);
    this->bassSlider.setBounds(382, 177, 60, 60);
    this->midSlider.setBounds(464, 177, 60, 60);
    this->trebleSlider.setBounds(551, 177, 60, 60);
    this->odSlider.setBounds(636, 177, 60, 60);
    this->odButton.setBounds(705, 174, 25, 25);
    this->volSlider.setBounds(292, 172, 70, 70);
    this->powerButton.setBounds(117, 170, 75, 75);
    this->powerLed.setBounds(205, 186, 40, 40);
    this->inputGain.setBounds(50, this->getBottom() - 30, 100, 25);
    this->outputGain.setBounds(215, this->getBottom() - 30, 100, 25);
}

