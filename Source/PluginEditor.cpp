/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

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
	setSize(1000, 1000); // 400

	// -----------------------------------------------
	// do the gain staging controls
	inGainSlider.setLookAndFeel(&jLookGain);
	driveSlider.setLookAndFeel(&jLookFreq);
	outGainSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(inGainSlider);
	addAndMakeVisible(driveSlider);
	addAndMakeVisible(outGainSlider);

	inGainSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INGAIN", inGainSlider);
	driveSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);
	outGainSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTGAIN", outGainSlider);

	// -----------------------------------------------
	lowFreqSlider.setLookAndFeel(&jLookGain);
	lowGainSlider.setLookAndFeel(&jLookFreq);
	lowQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(lowFreqSlider);
	addAndMakeVisible(lowGainSlider);
	addAndMakeVisible(lowQSlider);

	lowFreqSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWFREQ", lowFreqSlider);
	lowGainSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWGAIN", lowGainSlider);
	lowQSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWQ", lowQSlider);

	// -----------------------------------------------
	highFreqSlider.setLookAndFeel(&jLookGain);
	highGainSlider.setLookAndFeel(&jLookFreq);
	highQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(highFreqSlider);
	addAndMakeVisible(highGainSlider);
	addAndMakeVisible(highQSlider);

	highFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHFREQ", highFreqSlider);
	highGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHGAIN", highGainSlider);
	highQSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHQ", highQSlider);

	// -----------------------------------------------
	addAndMakeVisible(plotter);
	startTimer(100);

	background = juce::ImageCache::getFromMemory(BinaryData::Background_png, BinaryData::Background_pngSize);
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

void J13AudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a
	// solid colour)
	//g.fillAll(juce::Colours::darkslategrey);
	g.drawImageAt(background, 0, 0, false);

	//g.setColour(juce::Colours::black);
	g.setColour(juce::Colour::fromRGBA(0, 0, 0, 64));
	g.fillRect(topBottomDividerArea);

	plotter.repaint(plotArea);
}

void J13AudioProcessorEditor::layoutSizes()
{
	area = getLocalBounds();

	plotArea = area.removeFromTop(400);

	gainArea = area.removeFromTop(area.getHeight() / 3);
	inGainArea = gainArea.removeFromLeft(gainArea.getWidth() / 4);
	outGainArea = gainArea.removeFromRight(inGainArea.getWidth());
	driveArea = gainArea;

	driveArea.removeFromLeft((gainArea.getWidth() - inGainArea.getWidth()) / 2);
	driveArea.removeFromRight((gainArea.getWidth() - inGainArea.getWidth()) / 2);

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
}

void J13AudioProcessorEditor::resized()
{
	// The sequence of commands is significant, they can't be reordered because of
	// the .removeFrom* calls

	// TODO: save the dimension up front to make the calc easier to understand

	this->setLookAndFeel(&jLookBackground);

	layoutSizes();

	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	plotter.setBounds(plotArea);

	inGainSlider.setBounds(inGainArea);
	driveSlider.setBounds(driveArea);
	outGainSlider.setBounds(outGainArea);

	inGainSlider.showLabel(*this);
	driveSlider.showLabel(*this);
	outGainSlider.showLabel(*this);

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


void J13AudioProcessorEditor::timerCallback()
{
	stopTimer();

	auto x = audioProcessor.getCoeffs(0);
	if (x == nullptr) {
		if (!isTimerRunning())
			startTimer(100);
	} else {
		plotter.clearCoeffs();

		plotter.addCoeffs(x);
		plotter.addCoeffs(audioProcessor.getCoeffs(1));
	}
}