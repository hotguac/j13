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
	setSize(640, 640); // 400

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

	addAndMakeVisible(lowNormal);

	lowNormalAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWNORMAL", lowNormal);

	// -----------------------------------------------
	lowMidFreqSlider.setLookAndFeel(&jLookGain);
	lowMidGainSlider.setLookAndFeel(&jLookFreq);
	lowMidQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(lowMidFreqSlider);
	addAndMakeVisible(lowMidGainSlider);
	addAndMakeVisible(lowMidQSlider);

	lowMidFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "LOWMIDFREQ", lowMidFreqSlider);
	lowMidGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "LOWMIDGAIN", lowMidGainSlider);
	lowMidQSliderAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWMIDQ", lowMidQSlider);

	// -----------------------------------------------
	highMidFreqSlider.setLookAndFeel(&jLookGain);
	highMidGainSlider.setLookAndFeel(&jLookFreq);
	highMidQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(highMidFreqSlider);
	addAndMakeVisible(highMidGainSlider);
	addAndMakeVisible(highMidQSlider);

	highMidFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHMIDFREQ", highMidFreqSlider);
	highMidGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHMIDGAIN", highMidGainSlider);
	highMidQSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHMIDQ", highMidQSlider);

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

	lowMidFreqSlider.setLookAndFeel(nullptr);
	lowMidGainSlider.setLookAndFeel(nullptr);
	lowMidQSlider.setLookAndFeel(nullptr);

	highMidFreqSlider.setLookAndFeel(nullptr);
	highMidGainSlider.setLookAndFeel(nullptr);
	highMidQSlider.setLookAndFeel(nullptr);

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
	g.fillRect(inputLowDivider);
	g.fillRect(lowMidDivider);
	g.fillRect(lowDriveDivider);
	g.fillRect(driveHighDivider);
	g.fillRect(midHighDivider);
	g.fillRect(highOutputDivider);

	plotter.repaint(plotArea);
}

void J13AudioProcessorEditor::layoutSizes()
{
	area = getLocalBounds();

	plotArea = area.removeFromTop(area.getHeight() / 2.4);

	inputSection = area.removeFromLeft(area.getWidth() / 7.0f);
	int stripWidth = inputSection.getWidth();
	int stripHeight = inputSection.getHeight();
	int controlHeight = stripHeight / 3.5f;

	lowSection = area.removeFromLeft(stripWidth);
	lowMidSection = area.removeFromLeft(stripWidth);
	driveSection = area.removeFromLeft(stripWidth);
	highMidSection = area.removeFromLeft(stripWidth);
	highSection = area.removeFromLeft(stripWidth);
	outputSection = area;

	inputLowDivider = inputSection.removeFromRight(4);
	lowMidDivider = lowSection.removeFromRight(4);
	lowDriveDivider = lowMidSection.removeFromRight(4);
	driveHighDivider = driveSection.removeFromRight(4);
	midHighDivider = highMidSection.removeFromRight(4);
	highOutputDivider = highSection.removeFromRight(4);

	inputSection.removeFromTop(controlHeight / 2);
	inGainArea = inputSection.removeFromTop(controlHeight);

	driveSection.removeFromTop(controlHeight / 2);
	driveArea = driveSection.removeFromTop(controlHeight);

	outputSection.removeFromTop(controlHeight / 2.0f);
	outGainArea = outputSection.removeFromTop(controlHeight);


	lowFreqArea = lowSection.removeFromTop(controlHeight);
	lowGainArea = lowSection.removeFromTop(controlHeight);
	lowQArea = lowSection.removeFromTop(controlHeight);

	lowMidFreqArea = lowMidSection.removeFromTop(controlHeight);
	lowMidGainArea = lowMidSection.removeFromTop(controlHeight);
	lowMidQArea = lowMidSection.removeFromTop(controlHeight);

	highMidFreqArea = highMidSection.removeFromTop(controlHeight);
	highMidGainArea = highMidSection.removeFromTop(controlHeight);
	highMidQArea = highMidSection.removeFromTop(controlHeight);

	highFreqArea = highSection.removeFromTop(controlHeight);
	highGainArea = highSection.removeFromTop(controlHeight);
	highQArea = highSection.removeFromTop(controlHeight);
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

	lowNormal.setBounds(lowSection);

	lowMidFreqSlider.setBounds(lowMidFreqArea);
	lowMidGainSlider.setBounds(lowMidGainArea);
	lowMidQSlider.setBounds(lowMidQArea);

	highMidFreqSlider.setBounds(highMidFreqArea);
	highMidGainSlider.setBounds(highMidGainArea);
	highMidQSlider.setBounds(highMidQArea);

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
		plotter.addCoeffs(audioProcessor.getCoeffs(2));
		plotter.addCoeffs(audioProcessor.getCoeffs(3));
	}
}