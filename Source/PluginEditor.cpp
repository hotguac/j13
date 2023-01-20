/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
J13AudioProcessorEditor::J13AudioProcessorEditor(J13AudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // Set up the look and feel
  jLookFreq.fillColour = juce::Colours::darkblue;

  jLookRes.fillColour = juce::Colours::darkmagenta;
  jLookRes.outlineColour = juce::Colour(0xbfb0d8ff);
  jLookRes.lineColour = juce::Colours::black;

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  auto b = {400, 300};
  setSize(600, 400);

  // these define the parameters of our slider object
  inGainSlider.setRange(0.1, 1.2, 0.05);
  inGainSlider.setLookAndFeel(&jLookGain);
  addAndMakeVisible(inGainSlider);
  inGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INGAIN", inGainSlider);

  // these define the parameters of our slider object
  driveSlider.setRange(0.1, 1.2, 0.05);
  driveSlider.setLookAndFeel(&jLookFreq);
  addAndMakeVisible(driveSlider);
  driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);

  // these define the parameters of our slider object
  outGainSlider.setRange(0.1, 1.2, 0.05);
  outGainSlider.setLookAndFeel(&jLookRes);
  addAndMakeVisible(outGainSlider);
  outGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTGAIN", outGainSlider);
}

J13AudioProcessorEditor::~J13AudioProcessorEditor()
{
  // we need to reset look and feel here to prevent the look and feel set in resized()
  // from being deleted prior to this object being deleted.
  inGainSlider.setLookAndFeel(nullptr);
  driveSlider.setLookAndFeel(nullptr);
  outGainSlider.setLookAndFeel(nullptr);
  this->setLookAndFeel(nullptr);
}

//==============================================================================
void J13AudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(juce::Colours::darkslategrey);
  g.setColour(juce::Colours::darkgrey);

  juce::Rectangle<int> x = inGainArea;
  x.expand(0, 8);

  g.fillRect(x);
}

void J13AudioProcessorEditor::resized()
{
  // The sequence of commands is significant, they can't be reordered because of
  // the .removeFrom* calls

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
}