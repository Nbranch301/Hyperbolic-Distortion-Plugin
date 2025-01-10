/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce;

//==============================================================================
/**
*/

class GreenTheme : public LookAndFeel_V4
{
    public:
    GreenTheme()
    {
        setColour(Slider::rotarySliderOutlineColourId, Colour(213, 245, 223));
        setColour(Slider::thumbColourId, Colour(60, 81, 72));
        setColour(Slider::rotarySliderFillColourId, Colour(107, 142, 78));
        setColour(Slider::backgroundColourId, Colour(178, 197, 178));
        setColour(Slider::trackColourId, Colour(107, 142, 78));
        setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        
    }
    
};

class DistortionEffectProjectAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                        public Slider::Listener
{
public:
    DistortionEffectProjectAudioProcessorEditor (DistortionEffectProjectAudioProcessor&);
    ~DistortionEffectProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider *slider) override;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> driveAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> outputLvlAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> thresholdAttach;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DistortionEffectProjectAudioProcessor& audioProcessor;
    
    FontOptions chalkDuster;
    FontOptions trattatello;
    
    Slider driveSlider;
    Label driveLabel;
    
    Slider outputLvlSlider;
    Label outputLvlLabel;
    
    Slider wetSlider;
    Label wetLabel;
    
    Slider drySlider;
    Label dryLabel;
    
    Slider thresholdSlider;
    Label thresholdLabel;
    
    Label pluginTitle;
    Label pluginTitleShadow;
    
    GreenTheme greenTheme;
    
    const int borderX = 40;
    const int borderTop = 60;
    const int borderBottom = 5;
    const int sliderSize = 170;
    const int threshSliderXBound = 80.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionEffectProjectAudioProcessorEditor)
};
