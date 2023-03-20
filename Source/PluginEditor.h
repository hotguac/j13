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
	jRotary lowQSlider { "" };

	jRotary lowMidFreqSlider { "Freq" };
	jRotary lowMidGainSlider { "Gain" };
	jRotary lowMidQSlider { "Q" };

	jRotary highMidFreqSlider { "Freq" };
	jRotary highMidGainSlider { "Gain" };
	jRotary highMidQSlider { "Q" };

	jRotary highFreqSlider { "Freq" };
	jRotary highGainSlider { "Gain" };
	jRotary highQSlider { "" };

	jRotary lowPassSlider { "LowPass" };
	jRotary highPassSlider { "HighPass" };

	juce::TextButton lowBump { "Bump" };	// Q = 1.4f
	juce::TextButton lowNormal { "Shelf" }; // Q = 0.7f
	juce::TextButton lowWide { "Wide" };	// Q = 0.4f

	juce::TextButton highBump { "Bump" };	 // Q = 1.4f
	juce::TextButton highNormal { "Shelf" }; // Q = 0.7f
	juce::TextButton highWide { "Wide" };	 // Q = 0.4f

	juce::TextButton inputClean { "Clean" };
	juce::TextButton inputWarm { "Warm" };
	juce::TextButton inputBright { "Bright" };

	juce::TextButton outputClean { "Clean" };
	juce::TextButton outputWarm { "Warm" };
	juce::TextButton outputBright { "Thick" };

	juce::Font labelFont { LABELFONTSIZE };

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inputCleanAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inputWarmAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inputBrightAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> outputCleanAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> outputWarmAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> outputBrightAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowQAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowNormalAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowBumpAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowWideAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highNormalAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highBumpAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highWideAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidQAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidQAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highQAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassAttachment;

	FreqPlotter plotter;

	juce::Rectangle<int> area; // Total available to work with

	juce::Rectangle<int> plotArea; // Display of EQ curve

	// Major sections
	juce::Rectangle<int> inputSection;
	juce::Rectangle<int> lowSection;
	juce::Rectangle<int> midSection;
	juce::Rectangle<int> highSection;
	juce::Rectangle<int> outputSection;

	// Dividers
	juce::Rectangle<int> inputLowDivider;
	juce::Rectangle<int> lowMidDivider;
	juce::Rectangle<int> lowDriveDivider;
	juce::Rectangle<int> driveHighDivider;
	juce::Rectangle<int> midHighDivider;
	juce::Rectangle<int> highOutputDivider;

	// Individual controls
	juce::Rectangle<int> inGainArea;
	juce::Rectangle<int> inputCleanArea;
	juce::Rectangle<int> inputWarmArea;
	juce::Rectangle<int> inputBrightArea;

	juce::Rectangle<int> driveArea;

	juce::Rectangle<int> outGainArea;
	juce::Rectangle<int> outputCleanArea;
	juce::Rectangle<int> outputWarmArea;
	juce::Rectangle<int> outputThickArea;

	juce::Rectangle<int> lowFreqArea;
	juce::Rectangle<int> lowGainArea;

	juce::Rectangle<int> lowNormalArea;
	juce::Rectangle<int> lowBumpArea;
	juce::Rectangle<int> lowWideArea;

	juce::Rectangle<int> lowMidFreqArea;
	juce::Rectangle<int> lowMidGainArea;
	juce::Rectangle<int> lowMidQArea;

	juce::Rectangle<int> highMidFreqArea;
	juce::Rectangle<int> highMidGainArea;
	juce::Rectangle<int> highMidQArea;

	juce::Rectangle<int> highFreqArea;
	juce::Rectangle<int> highGainArea;

	// Hidden controls set by button choices
	juce::Rectangle<int> lowQArea;
	juce::Rectangle<int> highQArea;

	void timerCallback() override;

	juce::Image background;
	void layoutSizes();

	void lowBumpClicked();
	void lowNormalClicked();
	void lowWideClicked();

	void highBumpClicked();
	void highNormalClicked();
	void highWideClicked();

	juce::Rectangle<int> shrinkArea(juce::Rectangle<int> area);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessorEditor)
};


/*
TODO:

- Freq slider should display and move by 1Hz


- Add brushed aluminum look to knobs
- Add 3D look to knobs (top hat viewed directly), with darker edges and shiny middle
- Modify Input and Output Sections to have Clean; Warm; Bright saturation (xfmr) selection with 'lit' buttons
- For Mid Peak filters make the Q variable based on the gain, broad at low gain values and tighter at higher gain values
- Change High and Low Shelf Q to 'lit' buttons choosing normal, broad, or steep

Done-ish:
- Add brushed aluminum look to GUI background
- Add low-mid and high-mid peak filters
- Rearrange controls, left to right, to be Input Section; Low Shelf; Low Mid Peak; Drive Section; High Mid Peak; High Shelf; Output Section
- Adjust Freq to be restricted to 'sweet' zones and accomadate the new Mid Peak filters.

*/