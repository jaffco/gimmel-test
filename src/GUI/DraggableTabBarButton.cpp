#include "DraggableTabBarButton.h"
#include "DraggableTabbedComponent.h"

DraggableTabBarButton::DraggableTabBarButton(const juce::String& name, juce::TabbedButtonBar& ownerBar)
    : juce::TabBarButton(name, ownerBar)
{
}

DraggableTabBarButton::~DraggableTabBarButton()
{
}

void DraggableTabBarButton::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        isDragging = false;
        dragStartPosition = e.getPosition();
        juce::Logger::writeToLog("Mouse down on tab " + juce::String(getIndex()) + 
                                " at position (" + juce::String(e.getPosition().x) + 
                                "," + juce::String(e.getPosition().y) + ")");
    }
    
    // Call the parent class to handle normal tab selection
    juce::TabBarButton::mouseDown(e);
}

void DraggableTabBarButton::mouseDrag(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        auto dragDistance = e.getPosition().getDistanceFrom(dragStartPosition);
        
        // Start dragging after moving a few pixels to avoid accidental drags
        if (dragDistance > 5 && !isDragging)
        {
            isDragging = true;
            
            // Instead of starting drag from the button, notify the parent to start it
            auto tabIndex = getIndex();
            juce::Logger::writeToLog("Notifying parent to start drag for tab " + juce::String(tabIndex));
            
            // Find our parent DraggableTabbedComponent and have it start the drag
            if (auto* parent = findParentComponentOfClass<DraggableTabbedComponent>())
            {
                parent->startDragFromTab(tabIndex, this);
            }
        }
    }
    
    // Call the parent class
    juce::TabBarButton::mouseDrag(e);
}

void DraggableTabBarButton::mouseUp(const juce::MouseEvent& e)
{
    if (isDragging)
    {
        juce::Logger::writeToLog("Mouse up - drag ended for tab " + juce::String(getIndex()));
        isDragging = false;
    }
    
    // Call the parent class
    juce::TabBarButton::mouseUp(e);
}
