#include "MyLookAndFeel.h"

Knob::Knob() :juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, NoTextBox) {
    this->setLookAndFeel(&this->lookAndFeel);
    this->setPopupDisplayEnabled(true, false, nullptr);
}

void Knob::paint(juce::Graphics& g) {
    this->lookAndFeel.drawRotarySlider(g, this->getLocalBounds().getX(), this->getLocalBounds().getY(), this->getLocalBounds().getWidth(), this->getLocalBounds().getHeight(), this->getNormalisableRange().convertTo0to1(this->getValue()), 0.0f, 360.0f, *this);
}

juce::String Knob::getTextFromValue(double value){
    return juce::String((this->getNormalisableRange().convertTo0to1(value)) * 10.0f, 1);
}

Knob::~Knob() {
    this->setLookAndFeel(nullptr);
}



MyLookAndFeel::MyLookAndFeel() :LookAndFeel_V4() {
    this->knobImage = juce::ImageCache::getFromMemory(BinaryData::crush20l256_png, BinaryData::crush20l256_pngSize);
    this->knobFrames = static_cast<int>(juce::jmax(this->knobImage.getWidth(), this->knobImage.getHeight()) / juce::jmin(this->knobImage.getWidth(), this->knobImage.getHeight()));
    this->knobSize = juce::jmin(this->knobImage.getWidth(), this->knobImage.getHeight());
    this->setColour(juce::Slider::backgroundColourId, juce::Colours::grey.withAlpha(0.5f));
    this->setColour(juce::Slider::trackColourId, juce::Colours::black);
    this->setColour(juce::Slider::thumbColourId, juce::Colours::black);
    this->setColour(juce::Label::textColourId, juce::Colours::black);
}

void MyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) {
    if (this->knobImage.isValid()) {

        g.drawImage(this->knobImage, x, y, juce::jmin(width, height), juce::jmin(width, height), 0, 300 * juce::roundToInt(juce::jmap<float>(sliderPosProportional, 0.0f, static_cast<float>(this->knobFrames - 1))), this->knobSize, this->knobSize);

    }
    else {
        g.drawFittedText("No image", x, y, width, height, juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }

}

void MyLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle style, juce::Slider& slider){

    if(style==juce::Slider::SliderStyle::LinearHorizontal || style==juce::Slider::SliderStyle::LinearVertical)

    {
        auto trackWidth = juce::jmin(3.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::beveled, juce::PathStrokeType::square });

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint;

        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = { kx, ky };


        auto thumbWidth = 15.0f;

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        juce::ColourGradient thumbGradient(slider.findColour(juce::Slider::thumbColourId).contrasting(), maxPoint - juce::Point<float>(thumbWidth / 2, 0), slider.findColour(juce::Slider::thumbColourId), maxPoint - juce::Point<float>(0.7f * thumbWidth / 2, 0), true);
        g.setGradientFill(thumbGradient);
        g.fillEllipse(juce::Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(maxPoint));
    }
    else {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
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
        this->setImages(false, true, true, juce::ImageCache::getFromMemory(imageOffData, imageOffSize), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite, juce::Image(), 1.0f, juce::Colours::transparentWhite);
        this->repaint();
    }
    else {
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

VerticalSlider::VerticalSlider():juce::Slider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::NoTextBox){
    this->setLookAndFeel(&this->lookAndFeel);
    this->setPopupDisplayEnabled(true, false, nullptr);
}

void VerticalSlider::paint(juce::Graphics& g){
    Slider::paint(g);
}

VerticalSlider::~VerticalSlider(){
    this->setLookAndFeel(nullptr);
}

Label::Label():juce::Label(){
    this->setLookAndFeel(&lookAndFeel);
    this->setEditable(false);
}
Label::~Label() {
    this->setLookAndFeel(nullptr);
}
