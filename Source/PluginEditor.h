/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "FreqPlotter.h"
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

	jRotary lowMidFreqSlider { "Freq" };
	jRotary lowMidGainSlider { "Gain" };
	jRotary lowMidQSlider { "Q" };

	jRotary highMidFreqSlider { "Freq" };
	jRotary highMidGainSlider { "Gain" };
	jRotary highMidQSlider { "Q" };

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

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidFreqSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidGainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidQSliderAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidFreqSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidGainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidQSliderAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highFreqSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highGainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highQSliderAttachment;

	FreqPlotter plotter;

	juce::Rectangle<int> area; // Total available to work with

	juce::Rectangle<int> plotArea; // Display of EQ curve

	juce::Rectangle<int> inputSection;
	juce::Rectangle<int> lowSection;
	juce::Rectangle<int> lowMidSection;
	juce::Rectangle<int> driveSection;
	juce::Rectangle<int> highMidSection;
	juce::Rectangle<int> highSection;
	juce::Rectangle<int> outputSection;

	juce::Rectangle<int> inputLowDivider;
	juce::Rectangle<int> lowMidDivider;
	juce::Rectangle<int> lowDriveDivider;
	juce::Rectangle<int> driveHighDivider;
	juce::Rectangle<int> midHighDivider;
	juce::Rectangle<int> highOutputDivider;


	juce::Rectangle<int> inGainArea;
	juce::Rectangle<int> driveArea;
	juce::Rectangle<int> outGainArea;

	juce::Rectangle<int> lowFreqArea;
	juce::Rectangle<int> lowGainArea;
	juce::Rectangle<int> lowQArea;

	juce::Rectangle<int> lowMidFreqArea;
	juce::Rectangle<int> lowMidGainArea;
	juce::Rectangle<int> lowMidQArea;

	juce::Rectangle<int> highMidFreqArea;
	juce::Rectangle<int> highMidGainArea;
	juce::Rectangle<int> highMidQArea;

	juce::Rectangle<int> highFreqArea;
	juce::Rectangle<int> highGainArea;
	juce::Rectangle<int> highQArea;

	void timerCallback() override;

	juce::Image background;
	void layoutSizes();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessorEditor)
};


/*
TODO:

- Add low-mid and high-mid peak filters
- Rearrange controls, left to right, to be Input Section; Low Shelf; Low Mid Peak; Drive Section; High Mid Peak; High Shelf; Output Section
- Freq slider should display and move by 1Hz


- Add brushed aluminum look to knobs
- Add 3D look to knobs (top hat viewed directly), with darker edges and shiny middle
- Modify Input and Output Sections to have Clean; Warm; Bright saturation (xfmr) selection with 'lit' buttons
- Adjust Freq to be restricted to 'sweet' zones and accomadate the new Mid Peak filters.
- For Mid Peak filters make the Q variable based on the gain, broad at low gain values and tighter at higher gain values
- Change High and Low Shelf Q to 'lit' buttons choosing normal, broad, or steep

Done-ish:
- Add brushed aluminum look to GUI background

*/