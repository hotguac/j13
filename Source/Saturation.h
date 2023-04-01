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

class SaturationProcessor : public ProcessorBase {
public:
	SaturationProcessor() { }

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

		setFunction();

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
				channelData[sampleNum] = fn(x);
			}
		}
	}

	void reset() override { }

	enum SaturationType { clean = 0, warm = 1, bright = 2, thick = 3 };

	void setSaturationType(SaturationType sType) { activeType = sType; }

	SaturationType getSaturationType() { return activeType; }

private:
	int samplesPerBlock;
	SaturationType activeType = clean;

	float (*fn)(float);
	void setFunction()
	{
		// possible functions, needs tested/tweaked
		switch (activeType) {
		case clean:
			fn = [](float x) { return x; };
			break;
		case warm:
			fn = [](float x) {
				auto a = 0.2f * tanhf(x);
				auto b = 0.3f * sinf(x);
				auto M = 2;

				return 0.5f * tanhf(2 * (a + b) / M) + (a + b + x) / M;
			};
			break;
		case bright:
			fn = [](float x) {
				if (x >= 0.0f) {
					return tanhf(x) + (x * 0.25f);
				} else {
					return (0.8f * tanhf(x)) + (x * 0.25f);
				}
			};
			break;
		case thick:
			fn = [](float x) { return tanhf(x); };
			break;
		}
	}
};