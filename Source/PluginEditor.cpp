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

	createInputControls();
	createLowControls();
	createMidControls();
	createHighControls();
	createOutputControls();

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

	highPassSlider.setLookAndFeel(nullptr);
	lowPassSlider.setLookAndFeel(nullptr);

	this->setLookAndFeel(nullptr);
}


void J13AudioProcessorEditor::createInputControls()
{
	//
	inGainSlider.setLookAndFeel(&jLookGain);
	driveSlider.setLookAndFeel(&jLookFreq);

	addAndMakeVisible(inGainSlider);
	addAndMakeVisible(driveSlider);

	inGainAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INGAIN", inGainSlider);
	driveAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);

	addAndMakeVisible(inputClean);
	addAndMakeVisible(inputWarm);
	addAndMakeVisible(inputBright);

	inGainSlider.textFromValueFunction = [](double value) { return juce::String(value, 1); };
	driveSlider.textFromValueFunction = [](double value) { return juce::String(value, 1); };

	inGainSlider.updateText();
	driveSlider.updateText();
}

void J13AudioProcessorEditor::createLowControls()
{
	//
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
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWBUMP", lowBump);
	lowWideAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWWIDE", lowWide);

	lowBump.onClick = [this] { lowBumpClicked(); };
	lowNormal.onClick = [this] { lowNormalClicked(); };
	lowWide.onClick = [this] { lowWideClicked(); };

	lowFreqSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	lowFreqSlider.updateText();

	lowGainSlider.textFromValueFunction = [](double value) { return String(value, 1); };
	lowGainSlider.updateText();
}

void J13AudioProcessorEditor::createMidControls()
{
	//
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

	lowMidFreqSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	highMidFreqSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };

	lowMidGainSlider.textFromValueFunction = [](double value) { return juce::String(value, 1); };
	highMidGainSlider.textFromValueFunction = [](double value) { return juce::String(value, 1); };

	lowMidQSlider.textFromValueFunction = [](double value) { return juce::String(value, 1); };
	highMidQSlider.textFromValueFunction = [](double value) { return juce::String(value, 1); };

	lowMidFreqSlider.updateText();
	highMidFreqSlider.updateText();

	lowMidGainSlider.updateText();
	highMidGainSlider.updateText();
	lowMidQSlider.updateText();
	highMidQSlider.updateText();
}

void J13AudioProcessorEditor::createHighControls()
{
	highFreqSlider.setLookAndFeel(&jLookGain);
	highGainSlider.setLookAndFeel(&jLookFreq);
	highQSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(highFreqSlider);
	addAndMakeVisible(highGainSlider);

	highFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHFREQ", highFreqSlider);
	highGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHGAIN", highGainSlider);
	highQAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHQ", highQSlider);

	addAndMakeVisible(highNormal);
	addAndMakeVisible(highBump);
	addAndMakeVisible(highWide);

	highFreqSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	highFreqSlider.updateText();
}

void J13AudioProcessorEditor::createOutputControls()
{
	outGainSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(outGainSlider);

	outGainAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTGAIN", outGainSlider);

	addAndMakeVisible(outputClean);
	addAndMakeVisible(outputWarm);
	addAndMakeVisible(outputThick);

	highPassSlider.setLookAndFeel(&jLookRes);
	lowPassSlider.setLookAndFeel(&jLookRes);

	addAndMakeVisible(highPassSlider);
	addAndMakeVisible(lowPassSlider);

	highPassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHPASS", highPassSlider);
	lowPassAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWPASS", lowPassSlider);

	highPassSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	lowPassSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };

	highPassSlider.updateText();
	lowPassSlider.updateText();
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

	plotter.repaint(plotSection);
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

void J13AudioProcessorEditor::layoutSections()
{
	plotSection = area.removeFromTop(area.getHeight() / 2.4);

	inputSection = area.removeFromLeft(area.getWidth() / 5.8f);
	stripWidth = inputSection.getWidth();
	stripHeight = inputSection.getHeight();
	controlHeight = stripHeight / 4.0f; // 3.5f;

	lowSection = area.removeFromLeft(stripWidth);
	outputSection = area.removeFromRight(stripWidth);
	highSection = area.removeFromRight(stripWidth);
	midSection = area;

	inputLowDivider = inputSection.removeFromRight(4);
	lowMidDivider = lowSection.removeFromRight(4);
	midHighDivider = midSection.removeFromRight(4);
	highOutputDivider = highSection.removeFromRight(4);
}

void J13AudioProcessorEditor::layoutInput()
{
	inputSection.removeFromTop(controlHeight / 4);
	inGainArea = inputSection.removeFromTop(controlHeight);
	inputCleanArea = inputSection.removeFromTop(controlHeight / 3.2f);
	inputWarmArea = inputSection.removeFromTop(inputCleanArea.getHeight());
	inputBrightArea = inputSection.removeFromTop(inputCleanArea.getHeight());
	inputSection.removeFromTop(controlHeight / 4);

	driveArea = inputSection.removeFromTop(controlHeight);
}

void J13AudioProcessorEditor::layoutLow()
{
	lowFreqArea = lowSection.removeFromTop(controlHeight);
	lowGainArea = lowSection.removeFromTop(controlHeight / 1.05f);
	lowQArea = lowSection.removeFromTop(controlHeight / 1.1f);

	lowNormalArea = lowSection.removeFromTop(lowSection.getHeight() / 3.2f);
	lowBumpArea = lowSection.removeFromTop(lowNormalArea.getHeight());
	lowWideArea = lowSection.removeFromTop(lowNormalArea.getHeight());

	lowNormalArea = shrinkArea(lowNormalArea);
	lowBumpArea = shrinkArea(lowBumpArea);
	lowWideArea = shrinkArea(lowWideArea);
}

void J13AudioProcessorEditor::layoutMid()
{
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
}

void J13AudioProcessorEditor::layoutHigh()
{
	highFreqArea = highSection.removeFromTop(controlHeight);
	highGainArea = highSection.removeFromTop(controlHeight);
	//highQArea = highSection.removeFromTop(controlHeight);

	highNormalArea = highSection.removeFromTop(highSection.getHeight() / 3.2f);
	highBumpArea = highSection.removeFromTop(highNormalArea.getHeight());
	highWideArea = highSection.removeFromTop(highNormalArea.getHeight());
}

void J13AudioProcessorEditor::layoutOutput()
{
	outputSection.removeFromTop(controlHeight / 2.0f);
	outGainArea = outputSection.removeFromTop(controlHeight);
	outputCleanArea = outputSection.removeFromTop(controlHeight / 3.2f);
	outputWarmArea = outputSection.removeFromTop(outputCleanArea.getHeight());
	outputThickArea = outputSection.removeFromTop(outputCleanArea.getHeight());

	highPassArea = outputSection.removeFromTop(outputSection.getHeight() / 2.0f);
	lowPassArea = outputSection;
}

void J13AudioProcessorEditor::layoutSizes()
{
	area = getLocalBounds();

	layoutSections();
	layoutInput();
	layoutLow();
	layoutMid();
	layoutHigh();
	layoutOutput();

	inputCleanArea.removeFromLeft(22);
	inputCleanArea.removeFromRight(22);
	std::cout << "InputClean Button size = " << inputCleanArea.toString() << std::endl;
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
	plotter.setBounds(plotSection);

	inGainSlider.setBounds(inGainArea);
	inputClean.setBounds(inputCleanArea);
	inputWarm.setBounds(inputWarmArea);
	inputBright.setBounds(inputBrightArea);

	driveSlider.setBounds(driveArea);

	outGainSlider.setBounds(outGainArea);
	outputClean.setBounds(outputCleanArea);
	outputWarm.setBounds(outputWarmArea);
	outputThick.setBounds(outputThickArea);

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

	highNormal.setBounds(highNormalArea);
	highBump.setBounds(highBumpArea);
	highWide.setBounds(highWideArea);

	highPassSlider.setBounds(highPassArea);
	lowPassSlider.setBounds(lowPassArea);

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

	highPassSlider.showLabel(*this);
	lowPassSlider.showLabel(*this);
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