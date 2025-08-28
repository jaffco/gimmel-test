#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "DraggableTabBarButton.h"

// Forward declaration
class DraggableTabBarButton;

class DraggableTabbedComponent : public juce::TabbedComponent, public juce::DragAndDropContainer, public juce::DragAndDropTarget
{
public:
    explicit DraggableTabbedComponent(juce::TabbedButtonBar::Orientation orientation);
    ~DraggableTabbedComponent() override;
    
    void startDragFromTab(int tabIndex, juce::Component* sourceComponent);

protected:
    juce::TabBarButton* createTabButton(const juce::String& tabName, int tabIndex) override;

    // DragAndDropTarget methods
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDragMove(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDragExit(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    void itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

private:
    bool isDragOver = false;
    int calculateDropIndex(const juce::Point<int>& dropPos);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableTabbedComponent)
};
