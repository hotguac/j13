/*
  ==============================================================================

    GainProcessor.h
    Created: 22 Jan 2023 10:43:16am
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"

class GainProcessor : public ProcessorBase
{
public:
  GainProcessor()
  {
    gain.setGainDecibels(-12.0f);
  }

  void prepareToPlay(double sampleRate, int samplesPerBlock) override
  {
    juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2};
    gain.prepare(spec);
  }

  void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
  {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    gain.process(context);
  }

  void reset() override
  {
    gain.reset();
  }

  //...
  const juce::String getName() const override { return "Gain"; }

private:
  juce::dsp::Gain<float> gain;
};
