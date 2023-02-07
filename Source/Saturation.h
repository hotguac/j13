/*
  ==============================================================================

    Saturation.h
    Created: 24 Jan 2023 9:29:56am
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include "ProcessorBase.h"
#include <JuceHeader.h>

//===================================================================
class SaturationProcessor : public ProcessorBase {
public:
	//===================================================================
	SaturationProcessor() { }
	//...
	const juce::String getName() const override { return "SaturationFilter"; }

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		SaturationProcessor::samplesPerBlock = samplesPerBlock;
	}

	void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
	{
		juce::ScopedNoDenormals noDenormals;
		auto totalNumInputChannels = getTotalNumInputChannels();
		auto totalNumOutputChannels = getTotalNumOutputChannels();

		// In case we have more outputs than inputs, this code clears any output
		// channels that didn't contain input data, (because these aren't
		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
			buffer.clear(i, 0, buffer.getNumSamples());

		// This is the place where you'd normally do the guts of your plugin's
		// audio processing...
		for (int channel = 0; channel < totalNumInputChannels; ++channel) {
			auto* channelData = buffer.getWritePointer(channel);

			for (int sampleNum = 0; sampleNum < samplesPerBlock; ++sampleNum) {
				float x = channelData[sampleNum];
				// float Saturate(float input, float depth) { return input *
				// (1.0-depth+depth*(2.0-abs(input))) } channelData[sampleNum] =
				// std::tanh((std::tanh(x) + (0.9f * x))) + (0.2f * x);

				if (x >= 0.0f) {
					channelData[sampleNum] = std::tanh(x) + (x * 0.25);
				} else {
					channelData[sampleNum] = (0.8f * std::tanh(x)) + (x * 0.25);
				}
			}
		}
	}

	void reset() override { }

private:
	int samplesPerBlock;
};