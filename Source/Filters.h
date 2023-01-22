/*
  ==============================================================================

    Filters.h
    Created: 22 Jan 2023 10:57:54am
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessorBase.h"

class HighPassFilterProcessor : public ProcessorBase
{
public:
  HighPassFilterProcessor() {}
  //...
  const juce::String getName() const override { return "HighPassFilter"; }

  void prepareToPlay(double sampleRate, int samplesPerBlock) override
  {
    *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 1000.0f);

    juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2};
    filter.prepare(spec);
  }

  void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
  {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
  }

  void reset() override
  {
    filter.reset();
  }

private:
  juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};