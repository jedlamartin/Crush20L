/*
  ==============================================================================

    Switch.cpp
    Created: 2 Oct 2024 7:16:36pm
    Author:  Martin

  ==============================================================================
*/

#include "Switch.h"

Switch::Switch(const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize) :juce::ImageButton(), imageOffData(imageOffData), imageOffSize(imageOffSize), imageOnData(imageOnData), imageOnSize(imageOnSize) {

    this->setImages(false, true, true, juce::ImageCache::getFromMemory(imageOffData, imageOffSize), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite);
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

ToggleSwitch::ToggleSwitch(const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize) :
    Switch(imageOffData, imageOffSize, imageOnData, imageOnSize) {
    this->setClickingTogglesState(true);
}

void ToggleSwitch::configure() {
    this->onClick = [this]() {
        this->changeState();
        for (auto i : this->listeners) {
            i->changeState();
            bool temp = i->isToggleable();
            //DBG((temp ? "true" : "false"));
        }
        };

}

void ToggleSwitch::pushListener(Switch* toggleSwitch) {
    this->listeners.push_back(toggleSwitch);
}

ListenerSwitch::ListenerSwitch(const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize) :
    Switch(imageOffData, imageOffSize, imageOnData, imageOnSize) {
    this->setClickingTogglesState(false);
}

void ListenerSwitch::changeState() {
    this->setToggleState(!this->getToggleState(), juce::NotificationType::dontSendNotification);
    Switch::changeState();
}
