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

	inGainAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INGAIN", inGainSlider);
	driveAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);
	outGainAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTGAIN", outGainSlider);

	// -----------------------------------------------
	lowFreqSlider.setLookAndFeel(&jLookGain);
	lowGainSlider.setLookAndFeel(&jLookFreq);

	addAndMakeVisible(lowFreqSlider);
	addAndMakeVisible(lowGainSlider);

	lowFreqAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWFREQ", lowFreqSlider);
	lowGainAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWGAIN", lowGainSlider);
	lowQAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWQ", lowQSlider);

	addAndMakeVisible(lowNormal);
	addAndMakeVisible(lowBump);
	addAndMakeVisible(lowWide);

	lowNormalAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWNORMAL", lowNormal);
	lowBumpAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWNBUMP", lowBump);
	lowWideAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWWIDE", lowWide);

	lowBump.onClick = [this] { lowBumpClicked(); };
	lowNormal.onClick = [this] { lowNormalClicked(); };
	lowWide.onClick = [this] { lowWideClicked(); };

	// -----------------------------------------------
	lowMidFreqSlider.setLookAndFeel(&jLookGain);
	lowMidGainSlider.setLookAndFeel(&jLookFreq);
	lowMidQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(lowMidFreqSlider);
	addAndMakeVisible(lowMidGainSlider);
	addAndMakeVisible(lowMidQSlider);

	lowMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "LOWMIDFREQ", lowMidFreqSlider);
	lowMidGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "LOWMIDGAIN", lowMidGainSlider);
	lowMidQAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWMIDQ", lowMidQSlider);

	// -----------------------------------------------
	highMidFreqSlider.setLookAndFeel(&jLookGain);
	highMidGainSlider.setLookAndFeel(&jLookFreq);
	highMidQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(highMidFreqSlider);
	addAndMakeVisible(highMidGainSlider);
	addAndMakeVisible(highMidQSlider);

	highMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHMIDFREQ", highMidFreqSlider);
	highMidGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHMIDGAIN", highMidGainSlider);
	highMidQAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHMIDQ", highMidQSlider);

	// -----------------------------------------------
	highFreqSlider.setLookAndFeel(&jLookGain);
	highGainSlider.setLookAndFeel(&jLookFreq);
	highQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(highFreqSlider);
	addAndMakeVisible(highGainSlider);
	// addAndMakeVisible(highQSlider);

	highFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHFREQ", highFreqSlider);
	highGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHGAIN", highGainSlider);
	highQAttachment
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

juce::Rectangle<int> J13AudioProcessorEditor::shrinkArea(juce::Rectangle<int> area)
{
	//
	area.setWidth(area.getWidth() - 6);
	area.setLeft(area.getX() + 3);
	area.setHeight(area.getHeight() - 4);
	area.setTop(area.getY() + 2);

	return area;
}

void J13AudioProcessorEditor::lowBumpClicked()
{


	// Almost works but only checked at time of click, need to attach to gain slider


	std::cout << "lowBumpClicked" << std::endl;
	if (lowGainSlider.getValue() < 0.0f) {
		lowQSlider.setValue(1.4f, juce::NotificationType::sendNotification);
	} else {
		lowQSlider.setValue(0.7f, juce::NotificationType::sendNotification);
	}

	lowBump.setToggleable(true);
	lowBump.setColour(0, juce::Colours::blue);
	lowBump.setColour(1, juce::Colours::red);
}

void J13AudioProcessorEditor::lowNormalClicked()
{
	//
	std::cout << "lowNormalClicked" << std::endl;
	lowQSlider.setValue(0.7f, juce::NotificationType::sendNotification);

	lowBump.setToggleable(true);
	lowBump.setColour(0, juce::Colours::blue);
	lowBump.setColour(1, juce::Colours::red);
}

void J13AudioProcessorEditor::lowWideClicked()
{
	//
	std::cout << "lowWideClicked" << std::endl;
	lowQSlider.setValue(0.4f, juce::NotificationType::sendNotification);

	lowBump.setToggleable(true);
	lowBump.setColour(0, juce::Colours::blue);
	lowBump.setColour(1, juce::Colours::red);
}

void J13AudioProcessorEditor::layoutSizes()
{
	area = getLocalBounds();

	plotArea = area.removeFromTop(area.getHeight() / 2.4);

	inputSection = area.removeFromLeft(area.getWidth() / 5.8f);
	int stripWidth = inputSection.getWidth();
	int stripHeight = inputSection.getHeight();
	int controlHeight = stripHeight / 4.0f; // 3.5f;

	lowSection = area.removeFromLeft(stripWidth);
	outputSection = area.removeFromRight(stripWidth);
	highSection = area.removeFromRight(stripWidth);
	midSection = area;

	inputLowDivider = inputSection.removeFromRight(4);
	lowMidDivider = lowSection.removeFromRight(4);
	midHighDivider = midSection.removeFromRight(4);
	highOutputDivider = highSection.removeFromRight(4);

	inputSection.removeFromTop(controlHeight / 4);
	inGainArea = inputSection.removeFromTop(controlHeight);
	inputCleanArea = inputSection.removeFromTop(controlHeight / 3.2f);
	inputWarmArea = inputSection.removeFromTop(inputCleanArea.getHeight());
	inputBrightArea = inputSection.removeFromTop(inputCleanArea.getHeight());
	inputSection.removeFromTop(controlHeight / 4);

	driveArea = inputSection.removeFromTop(controlHeight);

	outputSection.removeFromTop(controlHeight / 2.0f);
	outGainArea = outputSection.removeFromTop(controlHeight);


	lowFreqArea = lowSection.removeFromTop(controlHeight);
	lowGainArea = lowSection.removeFromTop(controlHeight / 1.05f);
	lowQArea = lowSection.removeFromTop(controlHeight / 1.1f);

	lowNormalArea = lowSection.removeFromTop(lowSection.getHeight() / 3.2f);
	lowBumpArea = lowSection.removeFromTop(lowNormalArea.getHeight());
	lowWideArea = lowSection.removeFromTop(lowNormalArea.getHeight());

	lowNormalArea = shrinkArea(lowNormalArea);
	lowBumpArea = shrinkArea(lowBumpArea);
	lowWideArea = shrinkArea(lowWideArea);

	auto midWidth = midSection.getWidth();
	auto midHeight = midSection.getHeight();

	lowMidFreqArea = midSection.removeFromTop(controlHeight);
	lowMidGainArea = lowMidFreqArea.removeFromRight(midWidth / 2.0f);

	midSection.removeFromTop(8);

	auto saveArea = midSection;
	lowMidQArea = midSection.removeFromTop(controlHeight);
	lowMidQArea.removeFromRight(midWidth / 2.0f);
	midSection = saveArea;
	midSection.removeFromTop(controlHeight / 1.8f);

	highMidFreqArea = midSection.removeFromTop(controlHeight);
	highMidFreqArea.removeFromLeft(midWidth / 2.0);

	midSection.removeFromTop(8);
	highMidGainArea = midSection.removeFromTop(controlHeight);
	highMidQArea = highMidGainArea.removeFromRight(midWidth / 2.0f);

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

	lowNormal.setBounds(lowNormalArea);
	lowBump.setBounds(lowBumpArea);
	lowWide.setBounds(lowWideArea);

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

	lowMidFreqSlider.showLabel(*this);
	lowMidGainSlider.showLabel(*this);
	lowMidQSlider.showLabel(*this);

	highMidFreqSlider.showLabel(*this);
	highMidGainSlider.showLabel(*this);
	highMidQSlider.showLabel(*this);

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