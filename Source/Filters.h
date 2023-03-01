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

class HighPassProcessor : public ProcessorBase {
public:
	HighPassProcessor() { }

	const juce::String getName() const override { return "HighPassFilter"; }

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 200.0f);

		juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
		filter.prepare(spec);
	}

	void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
	{
		juce::dsp::AudioBlock<float> block(buffer);
		juce::dsp::ProcessContextReplacing<float> context(block);
		filter.process(context);
	}

	void reset() override { filter.reset(); }

	juce::dsp::IIR::Coefficients<float>* getCoeffs() { return filter.state.get(); }

private:
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};


//===================================================================
class LowShelfProcessor : public ProcessorBase {
public:
	LowShelfProcessor() { }

	const juce::String getName() const override { return "LowShelfFilter"; }

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 200.0f, 0.7f, 0.0f);

		juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
		filter.prepare(spec);
	}

	void updateSettings(int sampleRate, float freq, float q, float gain)
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, freq, q, gain);
	}

	void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
	{
		juce::dsp::AudioBlock<float> block(buffer);
		juce::dsp::ProcessContextReplacing<float> context(block);

		filter.process(context);
	}

	void reset() override { filter.reset(); }

	juce::dsp::IIR::Coefficients<float>* getCoeffs() { return filter.state.get(); }


private:
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};

//===================================================================
class HighShelfProcessor : public ProcessorBase {
public:
	HighShelfProcessor() { }

	const juce::String getName() const override { return "HighShelfFilter"; }

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 2000.0f, 0.7f, 1.0f);

		juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
		filter.prepare(spec);
	}

	void updateSettings(int sampleRate, float freq, float q, float gain)
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, freq, q, gain);
	}

	void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
	{
		juce::dsp::AudioBlock<float> block(buffer);
		juce::dsp::ProcessContextReplacing<float> context(block);

		filter.process(context);
	}

	void reset() override { filter.reset(); }

	juce::dsp::IIR::Coefficients<float>* getCoeffs() { return filter.state.get(); }

private:
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};

//===================================================================
class PeakProcessor : public ProcessorBase {
public:
	PeakProcessor() { }

	const juce::String getName() const override { return "PeakFilter"; }

	void prepareToPlay(double sampleRate, int samplesPerBlock) override
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 2000.0f, 0.7f, 1.0f);

		juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
		filter.prepare(spec);
	}

	void updateSettings(int sampleRate, float freq, float q, float gain)
	{
		*filter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, freq, q, gain);
	}

	void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
	{
		juce::dsp::AudioBlock<float> block(buffer);
		juce::dsp::ProcessContextReplacing<float> context(block);

		filter.process(context);
	}

	void reset() override { filter.reset(); }

private:
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};