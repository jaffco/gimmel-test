#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "../include/Gimmel/include/gimmel.hpp"
#include "Parameters.hpp"
#include "../media/test.h"

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

    ParameterBool chorusToggle { "chorusToggle" };
    ParameterFloat chorusRate { "chorusRate", 0.f, 20.f, 0.2f };
    ParameterFloat chorusDepth { "chorusDepth", 0.f, 50.f, 20.f };
    ParameterFloat chorusBlend { "chorusBlend", 0.f, 1.f, 0.5f };

    ParameterBool compressorToggle { "compressorToggle" };
    ParameterFloat compressorThreshold { "compressorThreshold", -60.f, 0.f, 0.f };
    ParameterFloat compressorRatio { "compressorRatio", 1.f, 20.f, 2.f };
    ParameterFloat compressorMakeup { "compressorMakeup", 0.f, 24.f, 0.f };
    ParameterFloat compressorKnee { "compressorKnee", 0.f, 5.f, 1.f };
    ParameterFloat compressorAttack { "compressorAttack", 0.f, 10.f, 3.5f };
    ParameterFloat compressorRelease { "compressorRelease", 0.f, 300.f, 100.f };

    ParameterBool delayToggle { "delayToggle" };
    ParameterFloat delayTime { "delayTime", 0.f, 3000.f, 398.f };
    ParameterFloat delayFeedback { "delayFeedback", 0.f, 1.f, 0.3f };
    ParameterFloat delayDamping { "delayDamping", 0.f, 1.f, 0.5f };
    ParameterFloat delayBlend { "delayBlend", 0.f, 1.f, 0.5f };

    ParameterBool detuneToggle { "detuneToggle" };
    ParameterFloat detunePitchRatio { "detunePitchRatio", 0.5f, 2.f, 1.f };
    ParameterFloat detuneWindowSize { "detuneWindowSize", 0.f, 300.f, 22.f };
    ParameterFloat detuneBlend { "detuneBlend", 0.f, 1.f, 0.5f };

    ParameterBool flangerToggle { "flangerToggle" };
    ParameterFloat flangerRate { "flangerRate", 0.f, 20.f, 0.2f };
    ParameterFloat flangerDepth { "flangerDepth", 0.f, 10.f, 5.f };
    ParameterFloat flangerBlend { "flangerBlend", 0.f, 1.f, 0.5f };

    ParameterBool phaserToggle { "phaserToggle" };
    ParameterFloat phaserRate { "phaserRate", 0.f, 20.f, 0.5f };
    ParameterFloat phaserFeedback { "phaserFeedback", -1.f, 1.f, 0.85f };

    ParameterBool reverbToggle { "reverbToggle" };
    ParameterFloat reverbTime { "reverbTime", 0.01f, 10.f, 0.03f };
    ParameterFloat reverbRegen { "reverbRegen", 0.f, 1.f, 0.3f };
    ParameterFloat reverbDamping { "reverbDamping", 0.f, 1.f, 0.5f };
    ParameterFloat reverbBlend { "reverbBlend", 0.f, 1.f, 0.5f };
    ParameterFloat reverbRoomLength { "reverbRoomLength", 1.f, 100.f, 50.f };
    ParameterFloat reverbAbsorptionCoefficient { "reverbAbsorptionCoefficient", 0.f, 1.f, 0.9f };

    ParameterBool tremoloToggle { "tremoloToggle" };
    ParameterFloat tremoloRate { "tremoloSpeed", 10.f, 2000.f, 1000.f };
    ParameterFloat tremoloDepth { "tremoloDepth", 0.f, 1.f, 0.5f };

    ParameterBool envelopeToggle { "envelopeToggle" };
    ParameterFloat envelopeQFactor { "envelopeQFactor", 0.1f, 20.f, 5.f };
    ParameterFloat envelopeAttackMs { "envelopeAttackMs", 0.f, 1000.f, 7.76f };
    ParameterFloat envelopeReleaseMs { "envelopeReleaseMs", 0.f, 2000.f, 1105.f };

    // Bundles are useful for grouping by effect to add tabs to the GUI
    ParameterBundle chorusParams{ &chorusToggle, &chorusRate, &chorusDepth, &chorusBlend };
    ParameterBundle compressorParams{ &compressorToggle, &compressorThreshold, &compressorRatio, &compressorMakeup, &compressorKnee, &compressorAttack, &compressorRelease }; 
    ParameterBundle delayParams{ &delayToggle, &delayTime, &delayFeedback, &delayDamping, &delayBlend };
    ParameterBundle detuneParams{ &detuneToggle, &detunePitchRatio, &detuneWindowSize, &detuneBlend };
    ParameterBundle flangerParams{ &flangerToggle, &flangerRate, &flangerDepth, &flangerBlend };
    ParameterBundle phaserParams{ &phaserToggle, &phaserRate, &phaserFeedback };
    ParameterBundle reverbParams{ &reverbToggle, &reverbTime, &reverbRegen, &reverbDamping, &reverbBlend, &reverbRoomLength, &reverbAbsorptionCoefficient };
    ParameterBundle tremoloParams{ &tremoloToggle, &tremoloRate, &tremoloDepth };
    ParameterBundle envelopeParams{ &envelopeToggle, &envelopeQFactor, &envelopeAttackMs, &envelopeReleaseMs };

    // Stack is useful for adding to the treeState
    ParameterStack fxParams{ &chorusParams, 
                            &compressorParams, 
                            &delayParams, 
                            &detuneParams, 
                            &flangerParams,
                            &phaserParams, 
                            &reverbParams,
                            &tremoloParams,
                            &envelopeParams };
    
    // public treeState?
    juce::AudioProcessorValueTreeState treeState;
    

private:
    //==============================================================================    // giml effects
    giml::EffectsLine<float> mEffectsLine;
    std::unique_ptr<giml::Chorus<float>> mChorus;
    std::unique_ptr<giml::Compressor<float>> mCompressor;
    std::unique_ptr<giml::Delay<float>> mDelay;
    std::unique_ptr<giml::Detune<float>> mDetune;
    std::unique_ptr<giml::Flanger<float>> mFlanger;
    std::unique_ptr<giml::Phaser<float>> mPhaser;
    std::unique_ptr<giml::Reverb<float>> mReverb;
    std::unique_ptr<giml::Tremolo<float>> mTremolo;
    std::unique_ptr<giml::EnvelopeFilter<float>> mEnvelope;

    // for wavfile
    int playHead = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
