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

	// cached images
	juce::Image background;

	// Look and feel must be before any component that uses it!!
	jLookAndFeel jLookGain;
	jLookAndFeel jLookFreq;
	jLookAndFeel jLookRes;
	jLookAndFeel jLookBackground;

	// Rotary Controls
	jRotary inGainSlider { "Input" };
	jRotary driveSlider { "Drive" };
	jRotary outGainSlider { "Output" };

	jRotary lowFreqSlider { "Freq" };
	jRotary lowGainSlider { "Gain" };

	jRotary lowMidFreqSlider { "Freq" };
	jRotary lowMidGainSlider { "Gain" };
	jRotary lowMidQSlider { "Q" };

	jRotary highMidFreqSlider { "Freq" };
	jRotary highMidGainSlider { "Gain" };
	jRotary highMidQSlider { "Q" };

	jRotary highFreqSlider { "Freq" };
	jRotary highGainSlider { "Gain" };

	jRotary highPassSlider { "HighPass" };

	// Buttons
	const int buttonWidth = 64;
	const int buttonHeight = 30;

	juce::TextButton lowBump { "Bump" };	// Q = 1.4f
	juce::TextButton lowShelf { "Shelf" };	// Q = 0.7f
	juce::TextButton lowWide { "Wide" };	// Q = 0.4f

	juce::TextButton highBump { "Bump" };	 // Q = 1.4f
	juce::TextButton highShelf { "Shelf" };	 // Q = 0.7f
	juce::TextButton highWide { "Wide" };	 // Q = 0.4f

	juce::TextButton inputClean { "Clean" };
	juce::TextButton inputWarm { "Warm" };
	juce::TextButton inputBright { "Bright" };

	juce::TextButton outputClean { "Clean" };
	juce::TextButton outputWarm { "Warm" };
	juce::TextButton outputThick { "Thick" };

	// Fonts
	juce::Font labelFont { LABELFONTSIZE };

	// Attachments to rotary controls, used to share with the AudioProcessor
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowGainAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidQAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidGainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highMidQAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highFreqAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highGainAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment;

	// Attachments to buttons, used to share with the AudioProcessor
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inputCleanAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inputWarmAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inputBrightAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> outputCleanAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> outputWarmAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> outputThickAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowShelfAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowBumpAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowWideAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highShelfAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highBumpAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highWideAttachment;

	// Frequency Response Plotter
	FreqPlotter plotter;

	// Total available to work with
	juce::Rectangle<int> area;

	// Major sections
	juce::Rectangle<int> plotSection;
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

	juce::Rectangle<int> midBottomDivider;
	juce::Rectangle<int> midMiddleDivider;
	juce::Rectangle<int> midTopDivider;

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

	juce::Rectangle<int> lowShelfArea;
	juce::Rectangle<int> lowBumpArea;
	juce::Rectangle<int> lowWideArea;

	juce::Rectangle<int> highShelfArea;
	juce::Rectangle<int> highBumpArea;
	juce::Rectangle<int> highWideArea;

	juce::Rectangle<int> lowMidFreqArea;
	juce::Rectangle<int> lowMidGainArea;
	juce::Rectangle<int> lowMidQArea;

	juce::Rectangle<int> highMidFreqArea;
	juce::Rectangle<int> highMidGainArea;
	juce::Rectangle<int> highMidQArea;

	juce::Rectangle<int> highFreqArea;
	juce::Rectangle<int> highGainArea;

	juce::Rectangle<int> highPassArea;

	// layout helpers
	int stripWidth;
	int stripHeight;
	int controlHeight;

	//
	void timerCallback() override;

	void layoutSizes();
	void layoutSections();
	void layoutInput();
	void layoutLow();
	void layoutMid();
	void layoutHigh();
	void layoutOutput();

	juce::Rectangle<int> shrinkArea(juce::Rectangle<int> area);

	// Create and attach controls
	void createInputControls();
	void createLowControls();
	void createMidControls();
	void createHighControls();
	void createOutputControls();

	static juce::String formatValue(double value)
	{
		char buffer[100];

		if (abs(value) > 99.9f) {
			snprintf(buffer, 100, "%6.0f", value);
			return (juce::String(buffer));
		}

		return juce::String(value);
	}

	juce::Rectangle<int> centerButtonArea(juce::Rectangle<int> buttonArea);

	enum JRadioGroups { input = 1, low = 2, high = 3, output = 4 };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessorEditor)
};


/*
TODO:
- Hook up input and out section buttons
- Build plot on a seperate canvas and paint to screen when finished 
- Only update plot display when values change
- make all buttons 'lit' when selected
- Add brushed aluminum look to knobs
- Add 3D look to knobs (top hat viewed directly), with darker edges and shiny middle

Done-ish:
- Draw dividers between low and high mid controls
- Rotate high-mid controls to mirror low-mid
- Lower high-mid frequency range to 1000 Hz
- Hook up shelf, bump. wide buttons to high shelf Q slider
- remove low pass filter
- Add high and low pass filters to PluginProcessor
- Add look to GUI background
- Add low-mid and high-mid peak filters
- Rearrange controls, left to right, to be Input Section; Low Shelf; Low Mid Peak; Drive Section; High Mid Peak; High Shelf; Output Section
- Adjust Freq to be restricted to 'sweet' zones and accomadate the new Mid Peak filters.
- Modify Input and Output Sections to have Clean; Warm; Bright saturation (xfmr) 
- Change High and Low Shelf Q to buttons choosing normal, broad, or steep
- All buttons should have same dimensions
- Adjust spacing of all controls

*/