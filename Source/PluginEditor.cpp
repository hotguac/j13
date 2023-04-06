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

	lowMidFreqSlider.setLookAndFeel(nullptr);
	lowMidGainSlider.setLookAndFeel(nullptr);
	lowMidQSlider.setLookAndFeel(nullptr);

	highMidFreqSlider.setLookAndFeel(nullptr);
	highMidGainSlider.setLookAndFeel(nullptr);
	highMidQSlider.setLookAndFeel(nullptr);

	highFreqSlider.setLookAndFeel(nullptr);
	highGainSlider.setLookAndFeel(nullptr);

	highPassSlider.setLookAndFeel(nullptr);

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

	inputClean.setRadioGroupId(input, juce::NotificationType::sendNotification);
	inputWarm.setRadioGroupId(input, juce::NotificationType::sendNotification);
	inputBright.setRadioGroupId(input, juce::NotificationType::sendNotification);

	inputClean.setClickingTogglesState(true);
	inputWarm.setClickingTogglesState(true);
	inputBright.setClickingTogglesState(true);

	inputCleanAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "INCLEAN", inputClean);
	inputWarmAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "INWARM", inputWarm);
	inputBrightAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "INBRIGHT", inputBright);

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

	addAndMakeVisible(lowShelf);
	addAndMakeVisible(lowBump);
	addAndMakeVisible(lowWide);

	lowShelfAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWSHELF", lowShelf);
	lowBumpAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWBUMP", lowBump);
	lowWideAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "LOWWIDE", lowWide);

	lowBump.setRadioGroupId(low, juce::NotificationType::sendNotification);
	lowShelf.setRadioGroupId(low, juce::NotificationType::sendNotification);
	lowWide.setRadioGroupId(low, juce::NotificationType::sendNotification);

	lowBump.setClickingTogglesState(true);
	lowShelf.setClickingTogglesState(true);
	lowWide.setClickingTogglesState(true);

	lowShelf.setToggleState(true, juce::NotificationType::sendNotification);

	lowFreqSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	lowFreqSlider.updateText();

	lowGainSlider.textFromValueFunction = [](double value) { return String(value, 1); };
	lowGainSlider.updateText();

	lowShelf.addListener(this);
	lowBump.addListener(this);
	lowWide.addListener(this);

	lowFreqSlider.addListener(this);
	lowGainSlider.addListener(this);
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

	lowMidGainSlider.addListener(this);
	highMidGainSlider.addListener(this);
	lowMidQSlider.addListener(this);
	highMidQSlider.addListener(this);
	lowMidFreqSlider.addListener(this);
	highMidFreqSlider.addListener(this);
}

void J13AudioProcessorEditor::createHighControls()
{
	highFreqSlider.setLookAndFeel(&jLookGain);
	highGainSlider.setLookAndFeel(&jLookFreq);

	addAndMakeVisible(highFreqSlider);
	addAndMakeVisible(highGainSlider);

	highFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHFREQ", highFreqSlider);
	highGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHGAIN", highGainSlider);

	addAndMakeVisible(highShelf);
	addAndMakeVisible(highBump);
	addAndMakeVisible(highWide);

	highShelfAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "HIGHSHELF", highShelf);
	highBumpAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "HIGHBUMP", highBump);
	highWideAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "HIGHWIDE", highWide);

	highBump.setRadioGroupId(high, juce::NotificationType::sendNotification);
	highShelf.setRadioGroupId(high, juce::NotificationType::sendNotification);
	highWide.setRadioGroupId(high, juce::NotificationType::sendNotification);

	highBump.setClickingTogglesState(true);
	highShelf.setClickingTogglesState(true);
	highWide.setClickingTogglesState(true);

	highShelf.setToggleState(true, juce::NotificationType::sendNotification);

	highFreqSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	highFreqSlider.updateText();

	highShelf.addListener(this);
	highBump.addListener(this);
	highWide.addListener(this);

	highFreqSlider.addListener(this);
	highGainSlider.addListener(this);
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

	outputClean.setRadioGroupId(output, juce::NotificationType::sendNotification);
	outputWarm.setRadioGroupId(output, juce::NotificationType::sendNotification);
	outputThick.setRadioGroupId(output, juce::NotificationType::sendNotification);

	outputClean.setClickingTogglesState(true);
	outputWarm.setClickingTogglesState(true);
	outputThick.setClickingTogglesState(true);

	outputCleanAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "OUTCLEAN", outputClean);
	outputWarmAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "OUTWARM", outputWarm);
	outputThickAttachment
		= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "OUTTHICK", outputThick);

	highPassSlider.setLookAndFeel(&jLookRes);
	addAndMakeVisible(highPassSlider);

	highPassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, "HIGHPASS", highPassSlider);

	highPassSlider.textFromValueFunction = [](double value) { return juce::String(rint(value)); };
	highPassSlider.updateText();
}

void J13AudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a
	// solid colour)
	//g.fillAll(juce::Colours::darkslategrey);
	g.drawImageAt(background, 0, 0, false);

	//g.setColour(juce::Colours::black);
	g.setColour(juce::Colour::fromRGBA(4, 0, 4, 80));
	g.fillRect(inputLowDivider);
	g.fillRect(lowMidDivider);
	g.fillRect(lowDriveDivider);
	g.fillRect(driveHighDivider);
	g.fillRect(midHighDivider);
	g.fillRect(highOutputDivider);
	g.fillRect(midBottomDivider);
	g.fillRect(midMiddleDivider);
	g.fillRect(midTopDivider);

	checkCoeffs();

	if (needRepaint) {
		needRepaint = false;
		plotter.repaint(plotSection);
		saveCoeffs();
	}
}

juce::Rectangle<int> J13AudioProcessorEditor::shrinkArea(juce::Rectangle<int> areaOld)
{
	//
	areaOld.setWidth(areaOld.getWidth() - 6);
	areaOld.setLeft(areaOld.getX() + 3);
	areaOld.setHeight(areaOld.getHeight() - 4);
	areaOld.setTop(areaOld.getY() + 2);

	return areaOld;
}

void J13AudioProcessorEditor::layoutSections()
{
	plotSection = area.removeFromTop(area.getHeight() / 2.4);

	area.removeFromTop(10);
	area.removeFromBottom(10);

	inputSection = area.removeFromLeft(area.getWidth() / 5.8f);
	stripWidth = inputSection.getWidth();
	stripHeight = inputSection.getHeight();
	controlHeight = stripHeight / 4.0f;

	lowSection = area.removeFromLeft(stripWidth);
	outputSection = area.removeFromRight(stripWidth);
	highSection = area.removeFromRight(stripWidth);
	midSection = area;

	inputLowDivider = inputSection.removeFromRight(4);
	lowMidDivider = lowSection.removeFromRight(4);
	midHighDivider = midSection.removeFromRight(4);
	highOutputDivider = highSection.removeFromRight(4);
}

juce::Rectangle<int> J13AudioProcessorEditor::centerButtonArea(juce::Rectangle<int> buttonArea)
{
	auto x = buttonArea.getX();
	auto y = buttonArea.getY();
	auto width = buttonArea.getWidth();
	auto height = buttonArea.getHeight();

	auto dx = (width - buttonWidth) / 2;
	auto dy = (height - buttonHeight) / 2;
	auto dw = width - buttonWidth;
	auto dh = height - buttonHeight;

	return juce::Rectangle<int>(x + dx, y + dy, width - dw, height - dh);
}

void J13AudioProcessorEditor::layoutInput()
{
	inGainArea = inputSection.removeFromTop(controlHeight);
	driveArea = inputSection.removeFromBottom(controlHeight);

	inputSection.removeFromTop(14);
	inputSection.removeFromBottom(14);

	inputCleanArea = inputSection.removeFromTop(inputSection.getHeight() / 3);
	inputWarmArea = inputSection.removeFromTop(inputCleanArea.getHeight());
	inputBrightArea = inputSection;

	inputCleanArea = centerButtonArea(inputCleanArea);
	inputWarmArea = centerButtonArea(inputWarmArea);
	inputBrightArea = centerButtonArea(inputBrightArea);
}

void J13AudioProcessorEditor::layoutLow()
{
	lowFreqArea = lowSection.removeFromTop(controlHeight);
	lowSection.removeFromTop(6);

	lowGainArea = lowSection.removeFromTop(controlHeight);
	lowSection.removeFromTop(6);

	lowSection.removeFromTop(10);

	lowShelfArea = lowSection.removeFromTop(lowSection.getHeight() / 4);
	lowBumpArea = lowSection.removeFromTop(lowShelfArea.getHeight());
	lowWideArea = lowSection.removeFromTop(lowShelfArea.getHeight());

	lowShelfArea = centerButtonArea(lowShelfArea);
	lowBumpArea = centerButtonArea(lowBumpArea);
	lowWideArea = centerButtonArea(lowWideArea);
}

void J13AudioProcessorEditor::layoutMid()
{
	auto midWidth = midSection.getWidth();
	// auto midHeight = midSection.getHeight();

	highMidFreqArea = midSection.removeFromTop(controlHeight);
	highMidGainArea = highMidFreqArea.removeFromRight(midWidth / 2.0f);

	midSection.removeFromTop(8);

	auto saveArea = midSection;
	highMidQArea = midSection.removeFromTop(controlHeight);
	highMidQArea.removeFromRight(midWidth / 2.0f);

	midSection = saveArea;
	midSection.removeFromTop(controlHeight / 1.8f);

	lowMidQArea = midSection.removeFromTop(controlHeight);
	lowMidQArea.removeFromLeft(midWidth / 2.0);

	midSection.removeFromTop(8);
	lowMidFreqArea = midSection.removeFromTop(controlHeight);
	lowMidGainArea = lowMidFreqArea.removeFromRight(midWidth / 2.0f);

	// juce::Rectangle<int>(x, y, w, h);
	auto x = lowMidFreqArea.getX();
	auto y = highMidQArea.getY() + highMidQArea.getHeight();
	y += (lowMidFreqArea.getY() - y) / 2;
	auto w = lowMidFreqArea.getWidth();
	auto h = 4;

	midBottomDivider = juce::Rectangle<int>(x, y, w, h);

	x = lowMidQArea.getX();
	y = highMidGainArea.getY() + highMidGainArea.getHeight();
	y += (lowMidQArea.getY() - y) / 2;
	w = lowMidQArea.getWidth();
	h = 4;

	midTopDivider = juce::Rectangle<int>(x, y, w, h);

	x = midTopDivider.getX();
	y = midTopDivider.getY();
	w = 4;
	h = midBottomDivider.getY() - midTopDivider.getY();

	midMiddleDivider = juce::Rectangle<int>(x, y, w, h);
}

void J13AudioProcessorEditor::layoutHigh()
{
	highFreqArea = highSection.removeFromTop(controlHeight);
	highSection.removeFromTop(6);

	highGainArea = highSection.removeFromTop(controlHeight);
	highSection.removeFromTop(6);

	highSection.removeFromTop(10);

	highShelfArea = highSection.removeFromTop(highSection.getHeight() / 4);
	highBumpArea = highSection.removeFromTop(highShelfArea.getHeight());
	highWideArea = highSection.removeFromTop(highShelfArea.getHeight());

	highShelfArea = centerButtonArea(highShelfArea);
	highBumpArea = centerButtonArea(highBumpArea);
	highWideArea = centerButtonArea(highWideArea);
}

void J13AudioProcessorEditor::layoutOutput()
{
	outGainArea = outputSection.removeFromTop(controlHeight);
	highPassArea = outputSection.removeFromBottom(controlHeight);

	outputSection.removeFromTop(14);
	outputSection.removeFromBottom(14);

	outputCleanArea = outputSection.removeFromTop(outputSection.getHeight() / 3);
	outputWarmArea = outputSection.removeFromTop(outputCleanArea.getHeight());
	outputThickArea = outputSection;

	outputCleanArea = centerButtonArea(outputCleanArea);
	outputWarmArea = centerButtonArea(outputWarmArea);
	outputThickArea = centerButtonArea(outputThickArea);
}

void J13AudioProcessorEditor::layoutSizes()
{
	area = getLocalBounds();

	// shrink for global buffer around edge
	area.removeFromTop(4);
	area.removeFromBottom(6);
	area.removeFromLeft(2);
	area.removeFromRight(2);

	layoutSections();
	layoutInput();
	layoutLow();
	layoutMid();
	layoutHigh();
	layoutOutput();
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

	lowShelf.setBounds(lowShelfArea);
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

	highShelf.setBounds(highShelfArea);
	highBump.setBounds(highBumpArea);
	highWide.setBounds(highWideArea);

	highPassSlider.setBounds(highPassArea);

	lowFreqSlider.showLabel(*this);
	lowGainSlider.showLabel(*this);

	lowMidFreqSlider.showLabel(*this);
	lowMidGainSlider.showLabel(*this);
	lowMidQSlider.showLabel(*this);

	highMidFreqSlider.showLabel(*this);
	highMidGainSlider.showLabel(*this);
	highMidQSlider.showLabel(*this);

	highFreqSlider.showLabel(*this);
	highGainSlider.showLabel(*this);

	highPassSlider.showLabel(*this);
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
		plotter.addCoeffs(audioProcessor.getCoeffs(4));

		saveCoeffs();
		needRepaint = true;
	}
}

void J13AudioProcessorEditor::sliderValueChanged(Slider* slider) { needRepaint = true; }

void J13AudioProcessorEditor::buttonClicked(Button*) { needRepaint = true; }

void J13AudioProcessorEditor::saveCoeffs()
{
	// TODO: save a copy of the all coeff values to be compared later
	if (audioProcessor.getCoeffs(0) == nullptr) {
		return;
	}

	for (auto filterNum = 0; filterNum < 5; ++filterNum) {
		auto filter = audioProcessor.getCoeffs(filterNum)->getRawCoefficients();
		for (auto coeffNum = 0; coeffNum < 6; ++coeffNum) {
			oldCoeff[filterNum][coeffNum] = filter[coeffNum];
		}
	}
}

void J13AudioProcessorEditor::checkCoeffs()
{
	// TODO: if any coeff values have changed then set the needrepaint
	// since there are smoothers involved, use a window abs(old-new) > limit
	auto limit = 0.0001f;

	if (audioProcessor.getCoeffs(0) == nullptr) {
		return;
	}

	for (auto filterNum = 0; filterNum < 5; ++filterNum) {
		auto filter = audioProcessor.getCoeffs(filterNum)->getRawCoefficients();
		for (auto coeffNum = 0; coeffNum < 6; ++coeffNum) {
			float diff = oldCoeff[filterNum][coeffNum] - filter[coeffNum];
			float adiff = abs(diff);
			if (adiff > limit) {
				needRepaint = true;
				std::cout << "in repaint " << filterNum << " " << coeffNum << " ";
				std::cout << adiff << std::endl;
			}
		}
	}
}
