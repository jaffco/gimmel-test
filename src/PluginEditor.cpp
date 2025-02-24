#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1080, 540);

    // init fxMenu
    chorusGui.attachParams(*p.fxParams[0], p.treeState);
    chorusGui.makeVisible();
    chorusGui.resized();

    compressorGui.attachParams(*p.fxParams[1], p.treeState);
    compressorGui.makeVisible();
    compressorGui.resized();

    mFxMenu.addEffect(chorusGui);
    mFxMenu.addEffect(compressorGui);
    addAndMakeVisible(&mFxMenu);

    for (auto& scope : processorRef.scopes) 
    {
        addAndMakeVisible(&scope);
    }
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    mFxMenu.setBounds(0, 0, bounds.getWidth() / 2, bounds.getHeight());
    int scopeHeight = bounds.getHeight() / processorRef.numScopes;
    for (size_t i = 0; i < processorRef.numScopes; ++i) 
    {
        processorRef.scopes[i].setBounds(bounds.getWidth() / 2, i * scopeHeight, bounds.getWidth() / 2, scopeHeight);
    }
}