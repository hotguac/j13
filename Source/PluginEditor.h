/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class J13AudioProcessorEditor : public juce::AudioProcessorEditor

{
public:
  J13AudioProcessorEditor(J13AudioProcessor &);
  ~J13AudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;
  // void sliderValueChanged(juce::Slider *slider) override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  J13AudioProcessor &audioProcessor;

  juce::Slider gainSlider;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessorEditor)
};
