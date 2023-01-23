/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
using Node = juce::AudioProcessorGraph::Node;

//==============================================================================
/**
 */
class J13AudioProcessor : public juce::AudioProcessor

{
public:
  //==============================================================================
  J13AudioProcessor();
  ~J13AudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;

  void releaseResources() override { mainProcessor->releaseResources(); }

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  //==============================================================================
  const juce::String getName() const override { return JucePlugin_Name; }

  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }

  double getTailLengthSeconds() const override { return 0.0; }

  //==============================================================================
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int index) override {}
  const juce::String getProgramName(int index) override { return {}; }
  void changeProgramName(int index, const juce::String &newName) override {}

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState apvts;

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

  std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

  Node::Ptr audioInputNode;
  Node::Ptr audioOutputNode;
  Node::Ptr midiInputNode;
  Node::Ptr midiOutputNode;

  Node::Ptr inputGainNode;
  Node::Ptr driveGainNode;
  Node::Ptr outputGainNode;

  Node::Ptr highPassNode;
  Node::Ptr lowShelfNode;

  void initialiseGraph();
  void updateGraph();
  void connectAudioNodes();
  void connectMidiNodes();

  double sampleRate;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J13AudioProcessor)
};
