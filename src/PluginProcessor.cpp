#include "PluginProcessor.h"
#include "PluginEditor.h"

// add params
juce::AudioProcessorValueTreeState::ParameterLayout
parameters(ParameterFloat& param) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameter_list;
    
    param.addToTree(parameter_list);
    
    return { parameter_list.begin(), parameter_list.end() };
}
//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "Parameters", parameters(mVolume))
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // init fx
    int sr = static_cast<int>(sampleRate);

    mChorus = std::make_unique<giml::Chorus<float>>(sr);
    mChorus->setParams();
    mEffectsLine.pushBack(mChorus.get());

    mCompressor = std::make_unique<giml::Compressor<float>>(sr);
    mCompressor->setParams();
    mEffectsLine.pushBack(mCompressor.get());

    mDelay = std::make_unique<giml::Delay<float>>(sr);
    mDelay->setParams();
    mEffectsLine.pushBack(mDelay.get());

    mDetune = std::make_unique<giml::Detune<float>>(sr);
    mDetune->setParams();
    mEffectsLine.pushBack(mDetune.get());

    mFlanger = std::make_unique<giml::Flanger<float>>(sr);
    mFlanger->setParams();
    mEffectsLine.pushBack(mFlanger.get());

    mPhaser = std::make_unique<giml::Phaser<float>>(sr);
    mPhaser->setParams();
    mEffectsLine.pushBack(mPhaser.get());

    mReverb = std::make_unique<giml::Reverb<float>>(sr);
    mReverb->setParams(0.03f, 0.3f, 0.5f, 0.5f, 50.f, 0.9f); // needs defaults 
    mEffectsLine.pushBack(mReverb.get());

    mTremolo = std::make_unique<giml::Tremolo<float>>(sr);
    mTremolo->setParams();
    mEffectsLine.pushBack(mTremolo.get());

    // init mAudioVisualizerComponent
    for (auto& scope : scopes) 
    {
        scope.setNumChannels(1);
        scope.setSamplesPerBlock(samplesPerBlock);
        scope.setBufferSize(static_cast<int>(sampleRate / samplesPerBlock));
    }
    scopes[1].setColours(juce::Colours::black, juce::Colours::blue);

    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

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

    // sample loop
    for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

        // feed input scope
        const float* input = buffer.getReadPointer(0, sample);;
        scopes[0].pushSample(input, 1);

        // calculate output sample
        //float out = mEffectsLine.processSample(*input);
        //float out = *input * giml::dBtoA(treeState.getRawParameterValue("volume")->load());
        float out = 0.f * treeState.getRawParameterValue("volume")->load();

        // write output to all channels
        for (int channel = 0; channel < totalNumInputChannels; channel++) 
        {
            buffer.getWritePointer(channel)[sample] = out;
        }

        // feed output scope
        const float* output = buffer.getReadPointer(0, sample);
        scopes[1].pushSample(output, 1);
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
