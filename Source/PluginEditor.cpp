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
  setSize(400, 300);

  // these define the parameters of our slider object
  inGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  inGainSlider.setRange(0.1, 1.2, 0.05);
  inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

  inGainSlider.setLookAndFeel(&jLookGain); // Testing this!!!!!!!!!!!!!!!!!

  addAndMakeVisible(inGainSlider);

  inGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INGAIN", inGainSlider);

  // these define the parameters of our slider object
  driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  driveSlider.setRange(0.1, 1.2, 0.05);
  driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  driveSlider.setLookAndFeel(&jLookFreq); // Testing this!!!!!!!!!!!!!!!!!

  addAndMakeVisible(driveSlider);

  driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);

  // these define the parameters of our slider object
  outGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  outGainSlider.setRange(0.1, 1.2, 0.05);
  outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
  outGainSlider.setLookAndFeel(&jLookRes); // Testing this!!!!!!!!!!!!!!!!!

  addAndMakeVisible(outGainSlider);

  outGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTGAIN", outGainSlider);
}

J13AudioProcessorEditor::~J13AudioProcessorEditor()
{
  // we need to reset look and feel here to prevent the look and feel set in resized()
  // from being deleted prior to this object being deleted.
  this->setLookAndFeel(nullptr);
}

//==============================================================================
void J13AudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);

  g.drawText("j13 Warmth", 10, 10, getWidth(), 40,
             juce::Justification::centredTop);
}

typedef juce::Rectangle<int> Rect;

void J13AudioProcessorEditor::resized()
{
  this->setLookAndFeel(&jLookBackground);

  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  Rect area = getLocalBounds();
  Rect gainArea = area.removeFromTop(area.getHeight() / 4);
  Rect inGainArea = gainArea.removeFromLeft(gainArea.getWidth() / 4);
  Rect outGainArea = gainArea.removeFromRight(inGainArea.getWidth());
  Rect driveArea = gainArea;

  driveArea.removeFromLeft((gainArea.getWidth() - inGainArea.getWidth()) / 2);
  driveArea.removeFromRight((gainArea.getWidth() - inGainArea.getWidth()) / 2);

  inGainSlider.setBounds(inGainArea);
  driveSlider.setBounds(driveArea);
  outGainSlider.setBounds(outGainArea);
}