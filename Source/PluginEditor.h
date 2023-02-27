/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "Plotter.h"
#include "PluginProcessor.h"
#include "jLookAndFeel.h"
#include "jRotary.h"

#include "JuceHeader.h"


class J13AudioProcessorEditor : public juce::AudioProcessorEditor,
								public juce::Timer

{
public:
	J13AudioProcessorEditor(J13AudioProcessor&);
	~J13AudioProcessorEditor() override;

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	J13AudioProcessor& audioProcessor;

	// Look and feel must be before any component that uses it!!
	jLookAndFeel jLookGain;
	jLookAndFeel jLookFreq;
	jLookAndFeel jLookRes;
	jLookAndFeel jLookBackground;

	jRotary inGainSlider { "Input" };
	jRotary driveSlider { "Drive" };
	jRotary outGainSlider { "Output" };

	jRotary lowFreqSlider { "Freq" };
	jRotary lowGainSlider { "Gain" };
	jRotary lowQSlider { "Q" };

	jRotary highFreqSlider { "Freq" };
	jRotary highGainSlider { "Gain" };
	jRotary highQSlider { "Q" };

	juce::Font labelFont { LABELFONTSIZE };

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainSliderAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowFreqSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowGainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowQSliderAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highFreqSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highGainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highQSliderAttachment;

	std::unique_ptr<Plotter> plotter;

	juce::Rectangle<int> area;
	juce::Rectangle<int> plotArea;
	juce::Rectangle<int> gainArea;
	juce::Rectangle<int> inGainArea;
	juce::Rectangle<int> outGainArea;
	juce::Rectangle<int> driveArea;

	juce::Rectangle<int> topBottomDividerArea;

	juce::Rectangle<int> lowFreqArea;
	juce::Rectangle<int> lowGainArea;
	juce::Rectangle<int> lowQArea;

	juce::Rectangle<int> highFreqArea;
	juce::Rectangle<int> highGainArea;
	juce::Rectangle<int> highQArea;

	void timerCallback() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessorEditor)
};