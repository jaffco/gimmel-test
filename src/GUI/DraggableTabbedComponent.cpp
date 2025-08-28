#include "../Parameters.hpp" // For FxMenu
#include <vector>
#include <string>
class FxMenu;
#include "DraggableTabbedComponent.h"

DraggableTabbedComponent::DraggableTabbedComponent(juce::TabbedButtonBar::Orientation orientation)
    : juce::TabbedComponent(orientation)
{
}

DraggableTabbedComponent::~DraggableTabbedComponent()
{
}

juce::TabBarButton* DraggableTabbedComponent::createTabButton(const juce::String& tabName, int tabIndex)
{
    return new DraggableTabBarButton(tabName, getTabbedButtonBar());
}

void DraggableTabbedComponent::startDragFromTab(int tabIndex, juce::Component* sourceComponent)
{
    
    // Start the drag operation from this component (which is a DragAndDropContainer)
    startDragging(juce::var(tabIndex), sourceComponent);
}

bool DraggableTabbedComponent::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    // Only accept drags from our own tab buttons
    bool interested = dragSourceDetails.sourceComponent != nullptr && 
                     dynamic_cast<DraggableTabBarButton*>(dragSourceDetails.sourceComponent.get()) != nullptr;
    
    
    return interested;
}

void DraggableTabbedComponent::itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    isDragOver = true;
    repaint();
}

void DraggableTabbedComponent::itemDragMove(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    // Calculate where the drop would land for visual feedback
    auto dropPos = dragSourceDetails.localPosition;
    int targetIndex = calculateDropIndex(dropPos);
    
    // Could highlight the target tab here
    // For now, just repaint to show drag is active
    repaint();
}

void DraggableTabbedComponent::itemDragExit(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    isDragOver = false;
    repaint();
}

void DraggableTabbedComponent::itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    isDragOver = false;
    repaint();
    
    // Get the source tab index
    if (auto* sourceButton = dynamic_cast<DraggableTabBarButton*>(dragSourceDetails.sourceComponent.get()))
    {
        int sourceIndex = sourceButton->getIndex();
        
        // Calculate the target index based on drop position
        auto dropPos = dragSourceDetails.localPosition;
        int targetIndex = calculateDropIndex(dropPos);
        
        // Debug output
        
        // Move the tab if the position changed
        if (targetIndex != sourceIndex && targetIndex >= 0 && targetIndex < getNumTabs())
        {
            moveTab(sourceIndex, targetIndex, true);
            // Notify tab order change if subclass implements emitTabOrderChanged
            if (auto* fxMenu = dynamic_cast<class FxMenu*>(this)) {
                fxMenu->emitTabOrderChanged();
            }
        }
        else
        {
        }
    }
}

int DraggableTabbedComponent::calculateDropIndex(const juce::Point<int>& dropPos)
{
    // Get the tab bar bounds
    auto tabBarBounds = getTabbedButtonBar().getBounds();
    
    // Convert drop position to tab bar coordinates
    auto tabBarPos = getTabbedButtonBar().getLocalPoint(this, dropPos);
    
    // For horizontal tabs, calculate based on X position
    if (getOrientation() == juce::TabbedButtonBar::TabsAtTop || 
        getOrientation() == juce::TabbedButtonBar::TabsAtBottom)
    {
        // Get the actual tab positions from the tab bar
        for (int i = 0; i < getNumTabs(); ++i)
        {
            if (auto* tab = getTabbedButtonBar().getTabButton(i))
            {
                auto tabBounds = tab->getBounds();
                if (tabBarPos.x < tabBounds.getRight())
                {
                    return i;
                }
            }
        }
        // If we get here, drop at the end
        return getNumTabs() - 1;
    }
    else
    {
        // For vertical tabs, calculate based on Y position
        for (int i = 0; i < getNumTabs(); ++i)
        {
            if (auto* tab = getTabbedButtonBar().getTabButton(i))
            {
                auto tabBounds = tab->getBounds();
                if (tabBarPos.y < tabBounds.getBottom())
                {
                    return i;
                }
            }
        }
        // If we get here, drop at the end
        return getNumTabs() - 1;
    }
}
