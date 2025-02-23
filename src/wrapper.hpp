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

// Forward declarations
class Parameter;
class ParameterBundle;
class EffectGui;

// Interface class
class Parameter {
protected: 
  std::string name;
  bool amToggle = false;

public:
  Parameter() {}
  virtual ~Parameter() {}

  std::string getName() { return this->name; }
  bool isToggle() { return this->amToggle; }
  virtual void addToTree(PARAM_LIST& pList) = 0;
  virtual void addToGui(EffectGui& gui, APVTS& treeState) = 0;
};

class ParameterBundle : public std::vector<Parameter*> {
public:

  ParameterBundle(std::initializer_list<Parameter*> params) {
    for (auto& p : params) {
      this->push_back(p);
    }
  }

  ParameterBundle(const ParameterBundle& pb) {
    for (auto& p : pb) {
      this->push_back(p);
    }
  }

  ParameterBundle& operator=(const ParameterBundle& pb) {
    if (this != &pb) {
      this->clear();
      for (auto& p : pb) {
        this->push_back(p);
      }
    }
    return *this;
  }
  
  ~ParameterBundle() {}

  void addToTree(PARAM_LIST& pList) {
    for (auto& param : *this) {
      param->addToTree(pList);
    }
  }

  void addToGui(EffectGui& gui, APVTS& treeState) {
    for (auto& param : *this) {
      param->addToGui(gui, treeState);
    }
  }

};

class EffectGui : public juce::Component {
private:
  std::unique_ptr<juce::ToggleButton> toggle;
  std::unique_ptr<BUTTON_ATTACHMENT> bAttachment;
  std::vector<std::unique_ptr<juce::Slider>> params;
  std::vector<std::unique_ptr<juce::Label>> labels;
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

    for (auto& l : labels) {
      addAndMakeVisible(l.get());
    } 
  }

  void resized() override {
    auto bounds = getLocalBounds();
    auto itemHeight = bounds.getHeight() / (params.size() + 1);

    toggle->setBounds(bounds.removeFromTop(itemHeight));
    for (int i = 0; i < params.size(); i++) {
      auto area = bounds.removeFromTop(itemHeight);
      labels[i]->setBounds(area.removeFromTop(20));
      params[i]->setBounds(area.removeFromTop(20));
    }
  }

  void paint(juce::Graphics& g) override {
    g.fillAll(juce::Colours::darkgrey);
  }

  void attachToggle(std::string name, APVTS& treeState) {
    bAttachment = MAKE_BUTTON(treeState, name, *toggle.get());
  }

  void attachParam(std::string name, APVTS& treeState) {
    params.push_back(std::make_unique<juce::Slider>());

    labels.push_back(std::make_unique<juce::Label>(name, name));
    auto& label = labels.back();
    label->setBorderSize ({ 1, 1, 1, 1 });
    label->setJustificationType(juce::Justification::centred);
    label->attachToComponent(params.back().get(), false);

    sAttachments.push_back(MAKE_SLIDER(treeState, name, *params.back().get()));
  }

  // TODO: memory safety
  void attachParams(ParameterBundle& params, APVTS& treeState) {
    for (auto& p : params) {
      if (p->isToggle()) {
        this->attachToggle(p->getName(), treeState);
      } else {
        this->attachParam(p->getName(), treeState);
      }
    }
  }

};

class ParameterBool : public Parameter {
private:
  bool value = false;

public:

  ParameterBool(std::string name, bool def = false) {
    this->amToggle = true;
    this->name = name;
    this->value = def;
  }

  void addToTree(PARAM_LIST& pList) override {
    pList.push_back(MAKE_PARAMB(this->name, this->name, value));
  }

  void addToGui(EffectGui& gui, APVTS& treeState) override {
    gui.attachToggle(this->name, treeState);
  }

};
  
class ParameterFloat : public Parameter {
private:
  float min = 0.f, max = 1.f, def = 0.f;

public:

  ParameterFloat(std::string name, float minVal, float maxVal, float def = 0.f) {
    this->name = name;
    this->min = minVal;
    this->max = maxVal;
    this->def = def;
  }

  void addToTree(PARAM_LIST& pList) override {
    pList.push_back(MAKE_PARAMF(this->name, this->name, min, max, def));
  }

  void addToGui(EffectGui& gui, APVTS& treeState) override {
    gui.attachParam(name, treeState);
  }

};

class FxMenu : public juce::TabbedComponent {
private:
public:

  void addEffect(EffectGui& eg) {
    addTab(eg.getName(), juce::Colours::darkolivegreen, &eg, true);
  }

};