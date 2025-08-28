#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Forward declaration
class DraggableTabbedComponent;

class DraggableTabBarButton : public juce::TabBarButton, public juce::DragAndDropContainer
{
public:
    DraggableTabBarButton(const juce::String& name, juce::TabbedButtonBar& ownerBar);
    ~DraggableTabBarButton() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    bool isDragging = false;
    juce::Point<int> dragStartPosition;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableTabBarButton)
};
