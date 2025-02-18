#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "../include/Gimmel/include/gimmel.hpp"

#define APVTS juce::AudioProcessorValueTreeState
#define PARAM_LIST std::vector<std::unique_ptr<juce::RangedAudioParameter>>
#define MAKE_PARAMF std::make_unique<juce::AudioParameterFloat>
#define MAKE_PARAMB std::make_unique<juce::AudioParameterBool>

#define SLIDER_ATTACHMENT juce::AudioProcessorValueTreeState::SliderAttachment
#define BUTTON_ATTACHMENT juce::AudioProcessorValueTreeState::ButtonAttachment
#define MAKE_SLIDER std::make_unique<SLIDER_ATTACHMENT>
#define MAKE_BUTTON std::make_unique<BUTTON_ATTACHMENT>


class EffectGui : public juce::Component {
private:
  std::unique_ptr<juce::ToggleButton> toggle;
  std::unique_ptr<BUTTON_ATTACHMENT> bAttachment;
  std::vector<std::unique_ptr<juce::Slider>> params;
  std::vector<std::unique_ptr<SLIDER_ATTACHMENT>> sAttachments;

public:

  EffectGui(std::string name) {
    this->setName(name);
    this->toggle = std::make_unique<juce::ToggleButton>();
  }

  void makeVisible() {
    addAndMakeVisible(toggle.get());

    for (auto& p : params) {
      addAndMakeVisible(p.get());
    }
  }

  void resized() override {
    auto bounds = getLocalBounds();
    auto itemHeight = bounds.getHeight() / (params.size() + 1);

    toggle->setBounds(bounds.removeFromTop(itemHeight));
    for (auto& p : params) {
      p->setBounds(bounds.removeFromTop(itemHeight));
    }
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(juce::Colours::darkgrey);
  }

  void attachToggle(std::string name, APVTS& treeState) {
    bAttachment = (MAKE_BUTTON(treeState, name, *toggle.get()));
  }

  void attachParam(std::string name, APVTS& treeState) {
    params.push_back(std::make_unique<juce::Slider>());
    sAttachments.push_back(MAKE_SLIDER(treeState, name, *params.back().get()));
  }

};

class FxMenu : public juce::TabbedComponent {
private:
public:


  void addEffect(EffectGui& eg) {
    addTab(eg.getName(), juce::Colours::darkolivegreen, &eg, true);
  }

};

class ParameterFloat {
private:
  std::string name;
  float min = 0.f, max = 1.f, def = 0.f;

public:

  ParameterFloat(std::string name, float minVal, float maxVal, float def = 0.f) {
    this->name = name;
    this->min = minVal;
    this->max = maxVal;
    this->def = def;
  }

  void addToTree(PARAM_LIST& pList) {
    pList.push_back(MAKE_PARAMF(name, name, min, max, def));
  }

  void addToGui(EffectGui& gui, APVTS& treeState) {
    gui.attachParam(name, treeState);
  }

};

class ParameterBool {
  private:
    std::string name = "";
    bool value = false;
  
  public:

    ParameterBool(std::string name, float def = false) {
      this->name = name;
      this->value = def;
    }
  
    void addToTree(PARAM_LIST& pList) {
      pList.push_back(MAKE_PARAMB(name, name, value));
    }
  
    void addToGui(EffectGui& gui, APVTS& treeState) {
      gui.attachToggle(name, treeState);
    }
  
  };