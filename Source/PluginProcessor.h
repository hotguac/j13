/*
  ==============================================================================

    PluginProcessor.h
    Created: Jan 2023
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
using Node = juce::AudioProcessorGraph::Node;

class J13AudioProcessor : public juce::AudioProcessor

{
public:
	J13AudioProcessor();
	~J13AudioProcessor() override;

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override { mainProcessor->releaseResources(); }
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; }

	const juce::String getName() const override { return JucePlugin_Name; }

	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	bool isMidiEffect() const override { return false; }

	double getTailLengthSeconds() const override { return 0.0; }

	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int index) override { }
	const juce::String getProgramName(int index) override { return {}; }
	void changeProgramName(int index, const juce::String& newName) override { }

	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	juce::AudioProcessorValueTreeState apvts;

	juce::dsp::IIR::Coefficients<float>* getCoeffs(int filterNum);

private:
	int count = 0;

	juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
	void updateGain(juce::StringRef ParameterID, juce::SmoothedValue<float>* smoother, Node::Ptr node);

	std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

	Node::Ptr audioInputNode;
	Node::Ptr audioOutputNode;
	Node::Ptr midiInputNode;
	Node::Ptr midiOutputNode;

	Node::Ptr inputGainNode;
	Node::Ptr driveNode;
	Node::Ptr driveOffsetNode;
	Node::Ptr outputGainNode;

	Node::Ptr highPassNode;
	Node::Ptr highShelfNode;
	Node::Ptr lowShelfNode;

	Node::Ptr highMidPeakNode;
	Node::Ptr lowMidPeakNode;

	Node::Ptr inSaturationNode;
	Node::Ptr outSaturationNode;

	void initialiseGraph();
	void updateGraph();
	void connectAudioNodes();
	void connectMidiNodes();

	double sampleRate;

	juce::SmoothedValue<float> smoothInGain { 1.0f };
	juce::SmoothedValue<float> smoothDrive { 1.0f };
	juce::SmoothedValue<float> smoothOutGain { 1.0f };

	juce::SmoothedValue<float> smoothHighPass { 1.0f };

	juce::SmoothedValue<float> smoothLowFreq { 100.0f };
	juce::SmoothedValue<float> smoothLowQ { 0.7f };
	juce::SmoothedValue<float> smoothLowGain { 1.0f };

	juce::SmoothedValue<float> smoothLowMidFreq { 100.0f };
	juce::SmoothedValue<float> smoothLowMidQ { 0.7f };
	juce::SmoothedValue<float> smoothLowMidGain { 1.0f };

	juce::SmoothedValue<float> smoothHighMidFreq { 4000.0f };
	juce::SmoothedValue<float> smoothHighMidQ { 0.7f };
	juce::SmoothedValue<float> smoothHighMidGain { 1.0f };

	juce::SmoothedValue<float> smoothHighFreq { 4000.0f };
	juce::SmoothedValue<float> smoothHighQ { 0.7f };
	juce::SmoothedValue<float> smoothHighGain { 1.0f };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessor)
};