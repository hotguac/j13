/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
J13AudioProcessorEditor::J13AudioProcessorEditor(J13AudioProcessor& p)
	: AudioProcessorEditor(&p)
	, audioProcessor(p)
{
	// Set up the look and feel
	jLookFreq.fillColour = juce::Colours::darkblue;

	jLookRes.fillColour = juce::Colours::darkmagenta;
	jLookRes.outlineColour = juce::Colour(0xbfb0d8ff);
	jLookRes.lineColour = juce::Colours::black;

	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(600, 400);

	// -----------------------------------------------
	// do the gain staging controls
	inGainSlider.setRange(-100.0f, 20.0f, 0.5);
	inGainSlider.setSkewFactorFromMidPoint(0.0f);
	inGainSlider.setLookAndFeel(&jLookGain);
	addAndMakeVisible(inGainSlider);
	inGainSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INGAIN", inGainSlider);

	driveSlider.setRange(0.1, 1.2, 0.05);
	driveSlider.setLookAndFeel(&jLookFreq);
	addAndMakeVisible(driveSlider);
	driveSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);

	outGainSlider.setRange(0.1, 1.2, 0.05);
	outGainSlider.setLookAndFeel(&jLookRes);
	addAndMakeVisible(outGainSlider);
	outGainSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTGAIN", outGainSlider);

	// -----------------------------------------------
	// do the low shelf controls

	lowFreqSlider.setRange(0.1, 1.2, 0.05);
	lowFreqSlider.setLookAndFeel(&jLookGain);
	addAndMakeVisible(lowFreqSlider);
	lowFreqSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWFREQ", lowFreqSlider);

	lowGainSlider.setRange(0.1, 1.2, 0.05);
	lowGainSlider.setLookAndFeel(&jLookFreq);
	addAndMakeVisible(lowGainSlider);
	lowGainSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWGAIN", lowGainSlider);

	lowQSlider.setRange(0.1, 1.2, 0.05);
	lowQSlider.setLookAndFeel(&jLookRes);
	addAndMakeVisible(lowQSlider);
	lowQSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWQ", lowQSlider);

	// -----------------------------------------------
	// high shelf controls

	highFreqSlider.setRange(0.1, 1.2, 0.05);
	highFreqSlider.setLookAndFeel(&jLookGain);
	addAndMakeVisible(highFreqSlider);
	highFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHFREQ", highFreqSlider);

	highGainSlider.setRange(0.1, 1.2, 0.05);
	highGainSlider.setLookAndFeel(&jLookFreq);
	addAndMakeVisible(highGainSlider);
	highGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHGAIN", highGainSlider);

	highQSlider.setRange(0.1, 1.2, 0.05);
	highQSlider.setLookAndFeel(&jLookRes);
	addAndMakeVisible(highQSlider);
	highQSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHQ", highQSlider);
}

J13AudioProcessorEditor::~J13AudioProcessorEditor()
{
	// we need to reset look and feel here to prevent the look and feel set in
	// resized() from being deleted prior to this object being deleted.
	inGainSlider.setLookAndFeel(nullptr);
	driveSlider.setLookAndFeel(nullptr);
	outGainSlider.setLookAndFeel(nullptr);

	lowFreqSlider.setLookAndFeel(nullptr);
	lowGainSlider.setLookAndFeel(nullptr);
	lowQSlider.setLookAndFeel(nullptr);

	highFreqSlider.setLookAndFeel(nullptr);
	highGainSlider.setLookAndFeel(nullptr);
	highQSlider.setLookAndFeel(nullptr);

	this->setLookAndFeel(nullptr);
}

//==============================================================================
void J13AudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a
	// solid colour)
	g.fillAll(juce::Colours::darkslategrey);

	g.setColour(juce::Colours::darkgrey);
	juce::Rectangle<int> x = inGainArea;
	x.expand(0, 8);
	g.fillRect(x);

	g.setColour(juce::Colours::black);
	g.fillRect(topBottomDividerArea);
}

void J13AudioProcessorEditor::resized()
{
	// The sequence of commands is significant, they can't be reordered because of
	// the .removeFrom* calls

	// TODO: save the dimension up front to make the calc easier to understand

	this->setLookAndFeel(&jLookBackground);

	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	area = getLocalBounds();
	gainArea = area.removeFromTop(area.getHeight() / 3);
	inGainArea = gainArea.removeFromLeft(gainArea.getWidth() / 4);
	outGainArea = gainArea.removeFromRight(inGainArea.getWidth());
	driveArea = gainArea;

	driveArea.removeFromLeft((gainArea.getWidth() - inGainArea.getWidth()) / 2);
	driveArea.removeFromRight((gainArea.getWidth() - inGainArea.getWidth()) / 2);

	inGainSlider.setBounds(inGainArea);
	driveSlider.setBounds(driveArea);
	outGainSlider.setBounds(outGainArea);

	inGainSlider.showLabel(*this);
	driveSlider.showLabel(*this);
	outGainSlider.showLabel(*this);

	// shrink the bottom part a little for spacing
	topBottomDividerArea = area.removeFromTop(area.getHeight() * 0.025f);
	area.removeFromBottom(area.getHeight() * 0.025f);

	// the left side is the low shelf area, right is high shelf
	Rectangle<int> left;
	left = area.removeFromLeft(area.getWidth() / 2);

	Rectangle<int> right;
	right = area;

	// calculate the low shelf areas
	lowFreqArea = left.removeFromLeft(left.getWidth() / 3);
	lowGainArea = left.removeFromLeft(left.getWidth() / 2);
	lowQArea = left;

	lowFreqArea.removeFromBottom(lowFreqArea.getHeight() * 0.6f);
	lowGainArea.removeFromTop(lowGainArea.getHeight() * 0.3f);
	lowGainArea.removeFromBottom(lowGainArea.getHeight() * (3.0f / 7.0f));

	lowQArea.removeFromTop(lowQArea.getHeight() * 0.6f);

	// calculate the high shelf areas
	highFreqArea = right.removeFromLeft(right.getWidth() / 3);
	highGainArea = right.removeFromLeft(right.getWidth() / 2);
	highQArea = right;

	highFreqArea.removeFromBottom(highFreqArea.getHeight() * 0.6f);
	highGainArea.removeFromTop(highGainArea.getHeight() * 0.3f);
	highGainArea.removeFromBottom(highGainArea.getHeight() * (3.0f / 7.0f));

	highQArea.removeFromTop(highQArea.getHeight() * 0.6f);

	lowFreqSlider.setBounds(lowFreqArea);
	lowGainSlider.setBounds(lowGainArea);
	lowQSlider.setBounds(lowQArea);

	highFreqSlider.setBounds(highFreqArea);
	highGainSlider.setBounds(highGainArea);
	highQSlider.setBounds(highQArea);

	lowFreqSlider.showLabel(*this);
	lowGainSlider.showLabel(*this);
	lowQSlider.showLabel(*this);

	highFreqSlider.showLabel(*this);
	highGainSlider.showLabel(*this);
	highQSlider.showLabel(*this);
}