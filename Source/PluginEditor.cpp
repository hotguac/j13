/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
J13AudioProcessorEditor::J13AudioProcessorEditor(J13AudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 300);

  // these define the parameters of our slider object
  gainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
  gainSlider.setRange(0.1, 1.2, 0.05);
  gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
  gainSlider.setPopupDisplayEnabled(true, false, this);
  gainSlider.setTextValueSuffix(" Volume");
  gainSlider.setValue(0.9);

  addAndMakeVisible(&gainSlider);
  gainSlider.addListener(this);
}

J13AudioProcessorEditor::~J13AudioProcessorEditor() {}

//==============================================================================
void J13AudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  // g.drawFittedText ("Hello World!", getLocalBounds(),
  // juce::Justification::centred, 1);
  g.drawText("j13 Warmth", 10, 10, getWidth(), 40,
             juce::Justification::centredTop);
}

void J13AudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  gainSlider.setBounds(80, 40, 40, 220);
}

void J13AudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
  if (slider == &gainSlider) {
    audioProcessor.setGain(gainSlider.getValue());
  }
}
