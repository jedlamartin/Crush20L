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
    this->backgound = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);

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


    
    setSize (1000, 350);

}

OrangeCrush20LAudioProcessorEditor::~OrangeCrush20LAudioProcessorEditor()
{
}

//==============================================================================
void OrangeCrush20LAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    if (this->backgound.isValid()) {
        g.drawImage(this->backgound, this->getLocalBounds().toFloat(), juce::RectanglePlacement::onlyReduceInSize | juce::RectanglePlacement::stretchToFit);
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
    this->odButton.setBounds(400, 100, 60, 60);
    this->volSlider.setBounds(292, 172, 70, 70);
}

Knob::Knob() :juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, TextBoxBelow) {
    this->setLookAndFeel(&this->lookAndFeel);
}

void Knob::paint(juce::Graphics& g){
    this->lookAndFeel.drawRotarySlider(g, this->getLocalBounds().getX(), this->getLocalBounds().getY(), this->getLocalBounds().getWidth(), this->getLocalBounds().getHeight(), this->getNormalisableRange().convertTo0to1(this->getValue()), 0.0f, 360.0f, *this);
}

Knob::~Knob(){
    this->setLookAndFeel(nullptr);
}



MyLookAndFeel::MyLookAndFeel() :LookAndFeel_V4() {
    this->knobImage = juce::ImageCache::getFromMemory(BinaryData::crush20l256_png, BinaryData::crush20l256_pngSize);
    this->knobFrames = static_cast<int>(juce::jmax(this->knobImage.getWidth(), this->knobImage.getHeight()) / juce::jmin(this->knobImage.getWidth(), this->knobImage.getHeight()));
    this->knobSize = juce::jmin(this->knobImage.getWidth(), this->knobImage.getHeight());
}

void MyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&){
    if (this->knobImage.isValid()) {
          
        g.drawImage(this->knobImage, x, y, juce::jmin(width, height), juce::jmin(width, height), 0, 300 * juce::roundToInt(juce::jmap<float>(sliderPosProportional, 0.0f, static_cast<float>(this->knobFrames - 1))), this->knobSize, this->knobSize);

    }
    else {
        g.drawFittedText("No image", x, y, width, height, juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }

}
