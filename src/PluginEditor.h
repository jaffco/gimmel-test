#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

struct ChorusSettings : public juce::Component
{
    ChorusSettings()
    {
        addAndMakeVisible (mToggleButton);
        addAndMakeVisible (mRateSlider);
        addAndMakeVisible (mDepthSlider);
        addAndMakeVisible (mBlendSlider);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto sliderHeight = bounds.getHeight() / 4;
        
        // Center the bypass button
        auto buttonHeight = sliderHeight / 2;
        auto buttonWidth = bounds.getWidth() / 2;
        mToggleButton.setBounds ((bounds.getWidth() - buttonWidth) / 2, 0, buttonWidth, buttonHeight);
        
        // Adjust the bounds for sliders to start below the bypass button
        bounds.removeFromTop(buttonHeight);
        mDepthSlider.setBounds (bounds.removeFromTop (sliderHeight));
        mRateSlider.setBounds (bounds.removeFromTop (sliderHeight));
        mBlendSlider.setBounds (bounds);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::darkgrey);
        
        // Add text for the bypass button
        g.setColour (juce::Colours::white);
        g.setFont (15.0f);
        g.drawText ("Toggle", mToggleButton.getBounds(), juce::Justification::centredBottom, true);
    }

    juce::ToggleButton mToggleButton;
    juce::Slider mRateSlider, mDepthSlider, mBlendSlider;
    std::unique_ptr
    <juce::AudioProcessorValueTreeState::SliderAttachment> 
    rateAttachment, depthAttachment, blendAttachment;
    std::unique_ptr
    <juce::AudioProcessorValueTreeState::ButtonAttachment>
    toggleAttachment;

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

    void attachParams(AudioPluginAudioProcessor& p) 
    {
        ChorusSettings* chorus = dynamic_cast<ChorusSettings*>(getTabContentComponent(0));
        chorus->rateAttachment = 
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.treeState, "chorusRate", chorus->mRateSlider);
        chorus->depthAttachment = 
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.treeState, "chorusDepth", chorus->mDepthSlider);
        chorus->blendAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.treeState, "chorusBlend", chorus->mBlendSlider);
        chorus->toggleAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            p.treeState, "chorusBypass", chorus->mToggleButton);
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
