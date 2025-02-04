#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

struct ChorusSettings : public juce::Component
{
    ChorusSettings()
    {
        addAndMakeVisible (mDepthSlider);
        addAndMakeVisible (mRateSlider);
        addAndMakeVisible (mMixSlider);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto sliderHeight = bounds.getHeight() / 3;
        mDepthSlider.setBounds (bounds.removeFromTop (sliderHeight));
        mRateSlider.setBounds (bounds.removeFromTop (sliderHeight));
        mMixSlider.setBounds (bounds);
    }

    juce::Slider mDepthSlider;
    juce::Slider mRateSlider;
    juce::Slider mMixSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusSettings)
};

struct FxMenu : public juce::TabbedComponent
{
    FxMenu (bool isRunningComponentTransformsDemo)
        : juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop)
    {
        auto colour = findColour (juce::ResizableWindow::backgroundColourId);

        addTab ("Chorus", colour, new ChorusSettings(), true);
        addTab ("Compressor", colour, new juce::Slider(), true);
        addTab ("Delay", colour, new juce::Slider(), true);
        addTab ("Detune", colour, new juce::Slider(), true);
        addTab ("Flanger", colour, new juce::Slider(), true);
        addTab ("Phaser", colour, new juce::Slider(), true);
        addTab ("Reverb", colour, new juce::Slider(), true);
        addTab ("Tremolo", colour, new juce::Slider(), true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxMenu)
};

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    FxMenu mFxMenu{false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
