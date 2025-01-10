/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
DistortionEffectProjectAudioProcessorEditor::DistortionEffectProjectAudioProcessorEditor (DistortionEffectProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    static Typeface::Ptr customChalkDusterFont = Typeface::createSystemTypefaceFor(BinaryData::Chalkduster_ttf, BinaryData::Chalkduster_ttfSize);
    chalkDuster = FontOptions(customChalkDusterFont);
    float sliderNameSize = 16.0f;
    float titleSize = 30.0f;
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 400);
    driveSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 60, 15);
    driveSlider.setRange(0.0, 30.0, 0.1);
    driveSlider.setTextValueSuffix(" dB");
//    driveSlider.setValue(0.0);
    driveSlider.addListener(this);
    driveSlider.setLookAndFeel(&greenTheme);
    addAndMakeVisible(driveSlider);
    
    driveAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "drive", driveSlider);
    
    driveLabel.setText("Drive", dontSendNotification);
    driveLabel.setFont(FontOptions(chalkDuster.withHeight(sliderNameSize)));
    driveLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(driveLabel);
    
    outputLvlSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    outputLvlSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 60, 15);
    outputLvlSlider.setTextValueSuffix(" dB");
    outputLvlSlider.setRange(-12.0, 12.0, 0.1);
//    outputLvlSlider.setValue(0.0);
    outputLvlSlider.setSkewFactorFromMidPoint(0.0);
    outputLvlSlider.addListener(this);
    outputLvlSlider.setLookAndFeel(&greenTheme);
    addAndMakeVisible(outputLvlSlider);
    
    outputLvlAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "outputLvl", outputLvlSlider);
    
    outputLvlLabel.setText("Output Lvl", dontSendNotification);
    outputLvlLabel.setFont(FontOptions(chalkDuster.withHeight(sliderNameSize)));
    outputLvlLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(outputLvlLabel);
    
    wetSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    wetSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 60, 15);
    wetSlider.setRange(0.0, 100.0, 1);
//    wetSlider.setValue(50);
    wetSlider.setTextValueSuffix("%");
    wetSlider.addListener(this);
    wetSlider.setLookAndFeel(&greenTheme);
    addAndMakeVisible(wetSlider);
    
    wetAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "wet", wetSlider);
    
    wetLabel.setText("Wet Mix", dontSendNotification);
    wetLabel.setFont(FontOptions(chalkDuster.withHeight(sliderNameSize)));
    wetLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(wetLabel);
    
    drySlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    drySlider.setTextBoxStyle(Slider::TextBoxAbove, false, 60, 15);
    drySlider.setRange(0.0, 100.0, 1);
//    drySlider.setValue(50);
    drySlider.setTextValueSuffix("%");
    drySlider.addListener(this);
    drySlider.setLookAndFeel(&greenTheme);
    addAndMakeVisible(drySlider);
    
    dryAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "dry", drySlider);
    
    dryLabel.setText("Dry Mix", dontSendNotification);
    dryLabel.setFont(FontOptions(chalkDuster.withHeight(sliderNameSize)));
    dryLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(dryLabel);
    
    thresholdSlider.setSliderStyle(Slider::LinearVertical);
    thresholdSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 60, 15);
    thresholdSlider.setRange(-20.0, 3.0, 0.1);
//    thresholdSlider.setValue(0.0);
    thresholdSlider.setSkewFactorFromMidPoint(-5.0);
    thresholdSlider.setTextValueSuffix("dB");
    thresholdSlider.addListener(this);
    thresholdSlider.setLookAndFeel(&greenTheme);
    addAndMakeVisible(thresholdSlider);
    
    thresholdAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "threshold", thresholdSlider);
    
    thresholdLabel.setText("Threshold", dontSendNotification);
    thresholdLabel.setFont(FontOptions(chalkDuster.withHeight(sliderNameSize)));
    thresholdLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(thresholdLabel);
    
    pluginTitleShadow.setText("Hyperbolic Distortion", dontSendNotification);
    pluginTitleShadow.setFont(FontOptions(chalkDuster.withHeight(titleSize)));
    pluginTitleShadow.setColour(Label::textColourId, Colour(40, 79, 35));
    pluginTitleShadow.setJustificationType(Justification::centred);
    addAndMakeVisible(pluginTitleShadow);
    
    pluginTitle.setText("Hyperbolic Distortion", dontSendNotification);
    pluginTitle.setFont(FontOptions(chalkDuster.withHeight(titleSize)));
    pluginTitle.setColour(Label::textColourId, Colour(76, 187, 23));
    pluginTitle.setJustificationType(Justification::centred);
    addAndMakeVisible(pluginTitle);
}

DistortionEffectProjectAudioProcessorEditor::~DistortionEffectProjectAudioProcessorEditor()
{
    driveAttach.reset();
    outputLvlAttach.reset();
    wetAttach.reset();
    dryAttach.reset();
    thresholdAttach.reset();
}

//==============================================================================
void DistortionEffectProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    int rectWidth = 30;
    g.fillAll(Colour(27, 39, 39));
    Rectangle <float> threshZeroMarker(getWidth()/2 - rectWidth/2, 192, rectWidth, 3);
    g.setColour(Colour(17, 25, 25));
    g.drawRect(threshZeroMarker);
    g.fillRect(threshZeroMarker);
}

void DistortionEffectProjectAudioProcessorEditor::resized()
{
    
    driveSlider.setBounds(borderX, borderTop, sliderSize, sliderSize);
    driveLabel.setBounds(driveSlider.getX(),
                         driveSlider.getY() + driveSlider.getHeight()/2,
                         sliderSize, driveLabel.getFont().getHeight());
    
    outputLvlSlider.setBounds(getWidth() - borderX - sliderSize, borderTop, sliderSize, sliderSize);
    outputLvlLabel.setBounds(outputLvlSlider.getX(),
                             outputLvlSlider.getY() + outputLvlSlider.getHeight()/2,
                             sliderSize, outputLvlLabel.getFont().getHeight());
    
    wetSlider.setBounds(borderX, getHeight() - borderBottom - sliderSize, sliderSize, sliderSize);
    wetLabel.setBounds(wetSlider.getX(),
                        wetSlider.getY() + wetSlider.getHeight()/2,
                        sliderSize, wetLabel.getFont().getHeight());
    
    drySlider.setBounds(getWidth() - borderX - sliderSize, getHeight() - borderBottom - sliderSize, sliderSize, sliderSize);
    dryLabel.setBounds(drySlider.getX(),
                        drySlider.getY() + drySlider.getHeight()/2,
                        sliderSize, dryLabel.getFont().getHeight());
    
    thresholdSlider.setBounds(getWidth()/2 - threshSliderXBound/2, driveSlider.getY() + driveSlider.getHeight()/2, threshSliderXBound, getHeight()/2);
    thresholdLabel.setBounds(thresholdSlider.getX(),
                             thresholdSlider.getY() + thresholdSlider.getHeight(),
                             threshSliderXBound, thresholdLabel.getFont().getHeight());

    pluginTitleShadow.setBounds(2, 2, getWidth(), borderTop - 7);
    pluginTitle.setBounds(0, 0, getWidth(), borderTop - 5);
}

void DistortionEffectProjectAudioProcessorEditor::sliderValueChanged(Slider *slider)
{

// Since I added parameter function, we don't need to update these variables in the process block this way  anymore
    
//    if(slider == &driveSlider){
//        //          dB = 20log(amp)
//        //  =>     amp = 10^(dB/20)
//        audioProcessor.drive = pow(10, driveSlider.getValue()/20);
//    } else if(slider == &outputLvlSlider){
//        audioProcessor.outputLvl = pow(10, outputLvlSlider.getValue()/20);
//    }else if(slider == &wetSlider){
//        audioProcessor.wet = wetSlider.getValue() / 100;
//    } else if(slider == &drySlider){
//        audioProcessor.dry = drySlider.getValue() / 100;
//    } else if(slider == &thresholdSlider){
//        audioProcessor.threshold = pow(10, thresholdSlider.getValue()/20);
//    }
}
