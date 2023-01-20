/*
  ==============================================================================

    jRotary.h
    Created: 19 Jan 2023 7:54:55pm
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include "jLookAndFeel.h"

#include <JuceHeader.h>

class jRotary : public juce::Slider
{
public:
  jRotary(juce::String labelText = "default") : lt(labelText)
  {
    setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 18);

    label.setText(lt, juce::NotificationType::dontSendNotification);
    label.setFont(labelFont);
    label.setJustificationType(juce::Justification::centred);
  };

  ~jRotary(){

  };

  void setBounds(Rectangle<int> newBounds)
  {
    bounds = newBounds;
    labelBounds = newBounds.removeFromTop(20);

    juce::Slider::setBounds(newBounds);
    label.setBounds(labelBounds);
  }

  void showLabel(juce::AudioProcessorEditor &p)
  {
    p.addAndMakeVisible(label);
  }

private:
  Rectangle<int> bounds;
  Rectangle<int> labelBounds;
  juce::Font labelFont{LABELFONTSIZE};
  juce::Label label;

  juce::String lt;
};