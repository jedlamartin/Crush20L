/*
  ==============================================================================

    Switch.h
    Created: 2 Oct 2024 7:16:36pm
    Author:  Martin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class Switch : public juce::ImageButton {
    const void* imageOffData;
    int imageOffSize;
    const void* imageOnData;
    int imageOnSize;
public:
    Switch(const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize);
    virtual void changeState();
};

class ToggleSwitch : public Switch {
private:
    std::vector<Switch*> listeners;
public:
    ToggleSwitch(const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize);
    void configure();
    void pushListener(Switch* toggleSwitch);
};

class ListenerSwitch : public Switch {
public:
    ListenerSwitch(const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize);
    void changeState() override;
};