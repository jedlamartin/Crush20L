#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

OrangeCrush20LAudioProcessor::OrangeCrush20LAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
    :
#endif
parameters(*this, nullptr, juce::Identifier("OrangeCeush20L"), this->createParameterLayout()),stage2Attachment(&stage2, this), 
            stage3Attachment(&stage3, this), stage4Attachment(&stage4, this), stage5Attachment(&stage5, this){
    this->stage2.initParameters(parameters.getRawParameterValue("gain"));
    this->parameters.addParameterListener("gain", &stage2Attachment);

    this->stage3.initParameters(parameters.getRawParameterValue("od"), parameters.getRawParameterValue("odButton"));
    this->parameters.addParameterListener("od", &stage3Attachment);
    this->parameters.addParameterListener("odButton", &stage3Attachment);

    this->stage4.initParameters(parameters.getRawParameterValue("bass"), parameters.getRawParameterValue("mid"), parameters.getRawParameterValue("treble"));
    this->parameters.addParameterListener("bass", &stage4Attachment);
    this->parameters.addParameterListener("mid", &stage4Attachment);
    this->parameters.addParameterListener("treble", &stage4Attachment);

    stage5.initParameters(parameters.getRawParameterValue("vol"));
    this->parameters.addParameterListener("vol", &stage5Attachment);


}

OrangeCrush20LAudioProcessor::~OrangeCrush20LAudioProcessor(){
}

const juce::String OrangeCrush20LAudioProcessor::getName() const{
    return JucePlugin_Name;
}

bool OrangeCrush20LAudioProcessor::acceptsMidi() const{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OrangeCrush20LAudioProcessor::producesMidi() const{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OrangeCrush20LAudioProcessor::isMidiEffect() const{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OrangeCrush20LAudioProcessor::getTailLengthSeconds() const{
    return 0.0;
}

int OrangeCrush20LAudioProcessor::getNumPrograms(){
    return 1;
}

int OrangeCrush20LAudioProcessor::getCurrentProgram(){
    return 0;
}

void OrangeCrush20LAudioProcessor::setCurrentProgram (int index){
}

const juce::String OrangeCrush20LAudioProcessor::getProgramName (int index){
    return {};
}

void OrangeCrush20LAudioProcessor::changeProgramName (int index, const juce::String& newName){
}

void OrangeCrush20LAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock){
    this->resample.configure(sampleRate);
    this->stage1.configure(sampleRate);
    this->stage2.configure(sampleRate);
    this->stage3.configure(sampleRate);
    this->stage4.configure(sampleRate);
    this->stage5.configure(sampleRate);
    this->stage6.configure(sampleRate);
}

void OrangeCrush20LAudioProcessor::releaseResources(){

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OrangeCrush20LAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OrangeCrush20LAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages){
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (this->parameters.getRawParameterValue("power")->load() >= 0.5f) {
        float inputGain = std::pow(10, parameters.getRawParameterValue("input")->load()/20.0f) * 3.0f;
        buffer.applyGain(inputGain);


        stage1.processBlock(buffer);


        resample.interpolate(buffer);


        resample.process([this](juce::AudioBuffer<float>& buffer) {
            stage2.processBlock(buffer);
            });
        resample.process([this](juce::AudioBuffer<float>& buffer) {
            stage3.processBlock(buffer);
            });
        resample.process([this](juce::AudioBuffer<float>& buffer) {
            stage4.processBlock(buffer);
            });
        resample.process([this](juce::AudioBuffer<float>& buffer) {
            stage5.processBlock(buffer);
            });
        resample.process([this](juce::AudioBuffer<float>& buffer) {
            stage6.processBlock(buffer);
            });

            
        resample.decimate(buffer);


        float outputGain = std::pow(10, parameters.getRawParameterValue("output")->load() / 20.0f) * 0.04f;

        buffer.applyGain(outputGain);
    }

}

bool OrangeCrush20LAudioProcessor::hasEditor() const{
    return true;
}

juce::AudioProcessorEditor* OrangeCrush20LAudioProcessor::createEditor(){
    return new OrangeCrush20LAudioProcessorEditor(*this, this->parameters);
}

//==============================================================================
void OrangeCrush20LAudioProcessor::getStateInformation (juce::MemoryBlock& destData){
    juce::MemoryOutputStream mos(destData, true);
    this->parameters.state.writeToStream(mos);
}

void OrangeCrush20LAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        this->parameters.replaceState(tree);

    }
}

juce::AudioProcessorValueTreeState::ParameterLayout OrangeCrush20LAudioProcessor::createParameterLayout(){
    juce::AudioProcessorValueTreeState::ParameterLayout vtsParameterLayout{
    std::make_unique<juce::AudioParameterBool>("power", "Power", false),
    std::make_unique<juce::AudioParameterFloat>("gain", "Gain", juce::NormalisableRange<float>(0.01f, 10000.0f, 0.05f, 0.3f), 0.01f),
    std::make_unique<juce::AudioParameterFloat>("bass", "Bass", juce::NormalisableRange<float>(0.01f, 250000.0f, 0.5f, 0.3f), 25000.0f),
    std::make_unique<juce::AudioParameterFloat>("mid", "Mid", juce::NormalisableRange<float>(0.01f, 25000.0f, 0.5f, 0.3f), 2500.000f),
    std::make_unique<juce::AudioParameterFloat>("treble", "Treble", juce::NormalisableRange<float>(0.01f, 250000.0f, 0.5f, 0.3f), 25000.0f),
    std::make_unique<juce::AudioParameterFloat>("od", "Overdrive", juce::NormalisableRange<float>(0.01f, 50000.0f, 0.5f, 6.58f), 0.01f),
    std::make_unique<juce::AudioParameterBool>("odButton", "Overdrive Button", false),
    std::make_unique<juce::AudioParameterFloat>("vol", "Volume", juce::NormalisableRange<float>(0.01f, 50000.0f, 0.5f, 0.3f), 0.01f),
    std::make_unique<juce::AudioParameterFloat>("input", "Input Gain", juce::NormalisableRange<float>(-24.0f,12.0f,0.1f), 0.0f),
    std::make_unique<juce::AudioParameterFloat>("output", "Output Gain",juce::NormalisableRange<float>(-12.0f,6.0f,0.1f), 0.0f)
    };
    return vtsParameterLayout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){
    return new OrangeCrush20LAudioProcessor();
}

