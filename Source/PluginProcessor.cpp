/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionEffectProjectAudioProcessor::DistortionEffectProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
//            drive(1.0),
//            outputLvl(1.0),
//            wet(0.5),
//            dry(0.5),
//            threshold(1.0),
            treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.state = ValueTree("saveParameters");
}

DistortionEffectProjectAudioProcessor::~DistortionEffectProjectAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout DistortionEffectProjectAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<AudioParameterFloat>("drive", "Drive", 0.0f, 30.0f, 0.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("outputLvl", "Output Level", -12.0f, 12.0f, 0.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("wet", "Wet Mix", 0.0f, 100.0f, 50.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("dry", "Dry Mix", 0.0f, 100.0f, 50.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("threshold", "Threshold", -20.0f, 3.0f, 1.0f));

    return {parameters.begin(), parameters.end()};
}

//==============================================================================
const juce::String DistortionEffectProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionEffectProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionEffectProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionEffectProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionEffectProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionEffectProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionEffectProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionEffectProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionEffectProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionEffectProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionEffectProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //dsp::ProcessSpec spec;
}

void DistortionEffectProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionEffectProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DistortionEffectProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for(int sample = 0; sample < buffer.getNumSamples(); sample++){
            // We have to first get the parameter values in case the user has used automation
            float drive = pow(10, *treeState.getRawParameterValue("drive")/20);
            float outputLvl = pow(10, *treeState.getRawParameterValue("outputLvl")/20);
            float wet = *treeState.getRawParameterValue("wet") / 100;
            float dry = *treeState.getRawParameterValue("dry") / 100;
            float threshold = pow(10, *treeState.getRawParameterValue("threshold")/20);
            
            // Here, I have used the hyperbolic tan function to distort the signal, as it has
            // horizontal asymptotes at y = -1 & y = 1. The variable drive therefore adjusts
            // the intensity (or harshness) of the wave
            
            float distortedSample = tanh(channelData[sample] * drive);
            
            if(channelData[sample] > threshold){
                channelData[sample] = threshold;
            } else if (channelData[sample] < -threshold){
                channelData[sample] = -threshold;
            }
            
            channelData[sample] = (dry * channelData[sample] + wet * distortedSample);
            
            channelData[sample] *=  outputLvl;
        }
    }
}

//==============================================================================
bool DistortionEffectProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionEffectProjectAudioProcessor::createEditor()
{
    return new DistortionEffectProjectAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionEffectProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(treeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DistortionEffectProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> savedParameters(getXmlFromBinary(data, sizeInBytes));
    if (savedParameters != nullptr)
    {
        // If xml has name "saveParameters"
        if(savedParameters -> hasTagName(treeState.state.getType()))
        {
            treeState.state = ValueTree::fromXml(*savedParameters);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionEffectProjectAudioProcessor();
}
