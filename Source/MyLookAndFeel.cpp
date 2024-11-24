/*
  ==============================================================================

    Knob.cpp
    Created: 24 Nov 2024 5:30:19pm
    Author:  Martin

  ==============================================================================
*/

#include "MyLookAndFeel.h"

Knob::Knob() :juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, TextBoxBelow) {
    this->setLookAndFeel(&this->lookAndFeel);
    this->setPopupDisplayEnabled(true, false, nullptr);
}

void Knob::paint(juce::Graphics& g) {
    this->lookAndFeel.drawRotarySlider(g, this->getLocalBounds().getX(), this->getLocalBounds().getY(), this->getLocalBounds().getWidth(), this->getLocalBounds().getHeight(), this->getNormalisableRange().convertTo0to1(this->getValue()), 0.0f, 360.0f, *this);
}

juce::String Knob::getTextFromValue(double value)
{
    //return juce::String((this->getNormalisableRange().convertTo0to1(value)) * 10.0f, 1);
    return juce::String(this->getValue());
}

Knob::~Knob() {
    this->setLookAndFeel(nullptr);
}



MyLookAndFeel::MyLookAndFeel() :LookAndFeel_V4() {
    this->knobImage = juce::ImageCache::getFromMemory(BinaryData::crush20l256_png, BinaryData::crush20l256_pngSize);
    this->knobFrames = static_cast<int>(juce::jmax(this->knobImage.getWidth(), this->knobImage.getHeight()) / juce::jmin(this->knobImage.getWidth(), this->knobImage.getHeight()));
    this->knobSize = juce::jmin(this->knobImage.getWidth(), this->knobImage.getHeight());
}

void MyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) {
    if (this->knobImage.isValid()) {

        g.drawImage(this->knobImage, x, y, juce::jmin(width, height), juce::jmin(width, height), 0, 300 * juce::roundToInt(juce::jmap<float>(sliderPosProportional, 0.0f, static_cast<float>(this->knobFrames - 1))), this->knobSize, this->knobSize);

    }
    else {
        g.drawFittedText("No image", x, y, width, height, juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }

}

int MyLookAndFeel::getSliderPopupPlacement(juce::Slider&)
{
    return juce::BubbleComponent::BubblePlacement::above;
}

Switch::Switch(std::atomic<float>* position, const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize) :juce::ImageButton(), imageOffData(imageOffData), imageOffSize(imageOffSize), imageOnData(imageOnData), imageOnSize(imageOnSize) {
    if (position->load()>0.5f) {
        this->setImages(false, true, true, juce::ImageCache::getFromMemory(imageOnData, imageOnSize), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite);
    }
    else {
        this->setImages(false, true, true, juce::ImageCache::getFromMemory(imageOffData, imageOffSize), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite);
    }
}

void Switch::changeState() {
    if (!this->getToggleState()) {
        //this->powerButton.setToggleState(false, juce::NotificationType::dontSendNotification);
        this->setImages(false, true, true, juce::ImageCache::getFromMemory(imageOffData, imageOffSize), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite);
        this->repaint();
    }
    else {
        //this->powerButton.setToggleState(true, juce::NotificationType::dontSendNotification);
        this->setImages(false, true, true, juce::ImageCache::getFromMemory(imageOnData, imageOnSize), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite);
        this->repaint();
    }

}

ToggleSwitch::ToggleSwitch(std::atomic<float>* position, const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize) :
    Switch(position, imageOffData, imageOffSize, imageOnData, imageOnSize) {
    this->setClickingTogglesState(true);
}

void ToggleSwitch::configure() {
    this->onClick = [this]() {
        this->changeState();
        for (auto i : this->listeners) {
            i->changeState();
        }
        };

}

void ToggleSwitch::pushListener(Switch* toggleSwitch) {
    this->listeners.push_back(toggleSwitch);
}

ListenerSwitch::ListenerSwitch(std::atomic<float>* position, const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize) :
    Switch(position, imageOffData, imageOffSize, imageOnData, imageOnSize) {
    this->setClickingTogglesState(false);
}

void ListenerSwitch::changeState() {
    this->setToggleState(!this->getToggleState(), juce::NotificationType::dontSendNotification);
    Switch::changeState();
}
