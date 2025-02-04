#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "../include/Gimmel/include/gimmel.hpp"

class Oscilloscope : public juce::Component
{
public:
    Oscilloscope()
    {
        setSize(800, 400); // Set initial size
        this->update();
    }

    void prepareToPlay(int sampleRate)
    {
        audioData[0].allocate(sampleRate);
        audioData[1].allocate(sampleRate);
    }

    void writeInputSample(float sample)
    {
        audioData[0].writeSample(sample);
    }

    void writeOutputSample(float sample)
    {
        audioData[1].writeSample(sample);
    }

    void paint(juce::Graphics& g) override
    {
        this->update();
        g.fillAll(juce::Colours::black);  // Background color

        // Draw input curve
        g.setColour(juce::Colours::white); // Line color
        g.strokePath(waveforms[0], juce::PathStrokeType(1.5f)); // Draw 

        // Draw output curve
        g.setColour(juce::Colours::blue); // Line color
        g.strokePath(waveforms[1], juce::PathStrokeType(1.5f)); // Draw
    }

    void resized() override
    {
        this->update();
        repaint();
    }

private:
    juce::Path waveforms[2]; 
    giml::CircularBuffer<float> audioData[2];

    // need to replace with oscope resize
    void update()
    {
        for (auto& curve : waveforms) { curve.clear(); }

        const int numSamples = audioData[0].size();  // Number of points
        const float width = getWidth();
        const float height = getHeight();

        for (int i = 0; i < 2; i++)
        {
            waveforms[i].startNewSubPath(0, height / 2.0f);
            for (int sample = 0; sample < numSamples; sample++)
            {
                float x = (static_cast<float>(sample) / numSamples) * width;
                float y = audioData[i].readSample(sample) * (height / 2.0f);
                waveforms[i].lineTo(x, y);
            }
        }
    }
};

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

    juce::AudioVisualiserComponent jOscilloscope{1};
    Oscilloscope mOscilloscope;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState treeState;
    giml::SinOsc<float> mOsc{48000};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
