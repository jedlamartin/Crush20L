/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
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
parameters(*this, nullptr, juce::Identifier("OrangeCeush20L"), {
    std::make_unique<juce::AudioParameterFloat>("gain", "Gain", juce::NormalisableRange<float>(10e-6, 10000.0f, 0.01, 0.3f), 10e-6),
    std::make_unique<juce::AudioParameterFloat>("bass", "Bass", juce::NormalisableRange<float>(10e-6, 250000.0f, 0.01, 0.3f), 25000.0f),
    std::make_unique<juce::AudioParameterFloat>("mid", "Mid", juce::NormalisableRange<float>(10e-6, 25000.0f, 0.01, 0.3f), 2500.000f),
    std::make_unique<juce::AudioParameterFloat>("treble", "Treble", juce::NormalisableRange<float>(10e-6, 250000.0f, 0.01, 0.3f), 25000.0f)
    })
{
    this->stage2.initParameters(parameters);
    this->stage4.initParameters(parameters);
}

OrangeCrush20LAudioProcessor::~OrangeCrush20LAudioProcessor()
{
}

//==============================================================================
const juce::String OrangeCrush20LAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrangeCrush20LAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OrangeCrush20LAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OrangeCrush20LAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OrangeCrush20LAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OrangeCrush20LAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OrangeCrush20LAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrangeCrush20LAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OrangeCrush20LAudioProcessor::getProgramName (int index)
{
    return {};
}

void OrangeCrush20LAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OrangeCrush20LAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->stage1.configure(sampleRate);
    this->stage2.configure(sampleRate);
    this->stage4.configure(sampleRate);
}

void OrangeCrush20LAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OrangeCrush20LAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OrangeCrush20LAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.


    // Stage 1
    //stage1.processBlock(buffer);
    stage4.processBlock(buffer);
}

//==============================================================================
bool OrangeCrush20LAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OrangeCrush20LAudioProcessor::createEditor()
{
    return new OrangeCrush20LAudioProcessorEditor(*this, this->parameters);
}

//==============================================================================
void OrangeCrush20LAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OrangeCrush20LAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrangeCrush20LAudioProcessor();
}
