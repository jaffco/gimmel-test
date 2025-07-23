//====================================================================================================
/* Joel A. Jaffe 2025-03-10

This header file defines and implements a set of classes that simplify the process of 
creating parameter objects and attaching them to the AudioProcessorValueTreeState and GUI.

*/
//====================================================================================================

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
class ParameterStack;
class EffectGui;

// Interface class
class Parameter {
protected: 
  std::string name;
  bool amToggle = false;
  bool amChoice = false;

public:
  Parameter() {}
  virtual ~Parameter() {}

  std::string getName() { return this->name; }
  bool isToggle() { return this->amToggle; }
  bool isChoice() { return this->amChoice; }
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

class ParameterStack : public std::vector<ParameterBundle*> {
  public:
  
    ParameterStack(std::initializer_list<ParameterBundle*> bundles) {
      for (auto& b : bundles) {
        this->push_back(b);
      }
    }
  
    ParameterStack(const ParameterStack& ps) {
      for (auto& pb : ps) {
        this->push_back(pb);
      }
    }
  
    ParameterStack& operator=(const ParameterStack& ps) {
      if (this != &ps) {
        this->clear();
        for (auto& pb : ps) {
          this->push_back(pb);
        }
      }
      return *this;
    }
  
    ~ParameterStack() {}
  
    void addToTree(PARAM_LIST& pList) {
      for (auto& bundle : *this) {
        bundle->addToTree(pList);
      }
    }
  
    void addToGui(EffectGui& gui, APVTS& treeState) {
      for (auto& bundle : *this) {
        bundle->addToGui(gui, treeState);
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
  std::vector<std::unique_ptr<juce::ComboBox>> choices;
  std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> cAttachments;

public:

  EffectGui(std::string name) {
    this->setName(name);
    this->toggle = std::make_unique<juce::ToggleButton>();
    this->toggle->setButtonText("Toggle");
  }

  void makeVisible() {
    addAndMakeVisible(toggle.get());

    for (auto& p : params) {
      addAndMakeVisible(p.get());
    }

    for (auto& l : labels) {
      addAndMakeVisible(l.get());
    } 

    for (auto& c : choices) {
      addAndMakeVisible(c.get());
    }
  }

  void resized() override {
    auto bounds = getLocalBounds();
    auto totalItems = params.size() + choices.size() + 1; // +1 for toggle
    auto itemHeight = bounds.getHeight() / totalItems;

    toggle->setBounds(bounds.removeFromTop(itemHeight));
    
    for (int i = 0; i < params.size(); i++) {
      auto area = bounds.removeFromTop(itemHeight);
      labels[i]->setBounds(area.removeFromTop(20));
      params[i]->setBounds(area.removeFromTop(20));
    }

    for (int i = 0; i < choices.size(); i++) {
      auto area = bounds.removeFromTop(itemHeight);
      // Choice labels are handled by attachChoice method
      choices[i]->setBounds(area);
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

  void attachChoice(std::string name, APVTS& treeState) {
    choices.push_back(std::make_unique<juce::ComboBox>());
    auto& comboBox = choices.back();
    
    // Get the parameter from the tree state to access its choices
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(treeState.getParameter(name))) {
        // Add all choices to the combo box
        for (int i = 0; i < param->choices.size(); ++i) {
            comboBox->addItem(param->choices[i], i + 1); // ComboBox items are 1-indexed
        }
    }
    
    // Set up the combo box
    comboBox->setTextWhenNothingSelected("Select " + name);
    comboBox->setTextWhenNoChoicesAvailable("No choices available");
    
    cAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(treeState, name, *comboBox));
  }

  // TODO: memory safety
  void attachParams(ParameterBundle& params, APVTS& treeState) {
    for (auto& p : params) {
      if (p->isToggle()) {
        this->attachToggle(p->getName(), treeState);
      } else if (p->isChoice()) {
        this->attachChoice(p->getName(), treeState);
      } else {
        this->attachParam(p->getName(), treeState);
      }
    }
  }

};

class FxMenu : public juce::TabbedComponent {
  public:
    FxMenu(bool vertical = true) : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop) {
      if (vertical) {
        setTabBarDepth(30);
      }
    }

    void addEffect(std::string name, ParameterBundle& params, APVTS& treeState) {
      auto eg = std::make_unique<EffectGui>(name);
      eg->attachParams(params, treeState);
      eg->makeVisible();
      eg->resized();
      addTab(name, juce::Colours::darkolivegreen, eg.release(), true);
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

    class ParameterChoice : public Parameter {
    private:
      juce::StringArray choices;
      int defaultIndex = 0;
    
    public:
    
      ParameterChoice(std::string name, juce::StringArray choiceList, int def = 0) {
        this->amChoice = true;
        this->name = name;
        this->choices = choiceList;
        this->defaultIndex = def;
      }
    
      void addToTree(PARAM_LIST& pList) override {
        pList.push_back(std::make_unique<juce::AudioParameterChoice>(this->name, this->name, choices, defaultIndex));
      }
    
      void addToGui(EffectGui& gui, APVTS& treeState) override {
        gui.attachChoice(name, treeState);
      }
    
    };