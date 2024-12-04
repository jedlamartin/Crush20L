#pragma once
#include "JuceHeader.h"
#include <vector>

class MyLookAndFeel :public juce::LookAndFeel_V4 {
private:
    juce::Image knobImage;
    int knobFrames;
    int knobSize;
public:
    MyLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle, juce::Slider&) override;
    int getSliderPopupPlacement(juce::Slider&) override;
};

class Knob :public juce::Slider {
private:
    MyLookAndFeel lookAndFeel;
public:
    Knob();
    void paint(juce::Graphics& g) override;
    juce::String getTextFromValue(double value) override;
    ~Knob();
};

class VerticalSlider : public juce::Slider {
private:
    MyLookAndFeel lookAndFeel;
public:
    VerticalSlider();
    void paint(juce::Graphics& g) override;
    ~VerticalSlider();
};

class Switch : public juce::ImageButton {
    const void* imageOffData;
    int imageOffSize;
    const void* imageOnData;
    int imageOnSize;
public:
    Switch(std::atomic<float>* position, const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize);
    virtual void changeState();
};

class ToggleSwitch : public Switch {
private:
    std::vector<Switch*> listeners;
public:
    ToggleSwitch(std::atomic<float>* position, const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize);
    void configure();
    void pushListener(Switch* toggleSwitch);
};

class ListenerSwitch : public Switch {
public:
    ListenerSwitch(std::atomic<float>*position, const void* imageOffData, int imageOffSize, const void* imageOnData, int imageOnSize);
    void changeState() override;
};

class Label : public juce::Label {
private:
    MyLookAndFeel lookAndFeel;
public:
    Label();
    ~Label();
};