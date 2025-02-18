#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "../include/Gimmel/include/gimmel.hpp"
#include "wrapper.hpp"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // input, output, and spectral scopes
    static const size_t numScopes = 2;
    juce::AudioVisualiserComponent scopes[2] { { 1 }, { 1 } };

    ParameterFloat mVolume{"volume", -96.f, 6.f, -96.f};

    // public treeState?
    juce::AudioProcessorValueTreeState treeState;
    

private:
    //==============================================================================

    // giml effects
    giml::EffectsLine<float> mEffectsLine;
    std::unique_ptr<giml::Chorus<float>> mChorus;
    std::unique_ptr<giml::Compressor<float>> mCompressor;
    std::unique_ptr<giml::Delay<float>> mDelay;
    std::unique_ptr<giml::Detune<float>> mDetune;
    std::unique_ptr<giml::Flanger<float>> mFlanger;
    std::unique_ptr<giml::Phaser<float>> mPhaser;
    std::unique_ptr<giml::Reverb<float>> mReverb;
    std::unique_ptr<giml::Tremolo<float>> mTremolo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
