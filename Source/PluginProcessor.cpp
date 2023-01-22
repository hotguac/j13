/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GainProcessor.h"
#include "Filters.h"

//==============================================================================
J13AudioProcessor::J13AudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameters()),
      mainProcessor(new juce::AudioProcessorGraph())
{
}

J13AudioProcessor::~J13AudioProcessor() {}

bool J13AudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const
{
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;

  return true;
}

void J13AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                     juce::MidiBuffer &midiMessages)
{
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  updateGraph();

  mainProcessor->processBlock(buffer, midiMessages);

  // see https://www.youtube.com/watch?v=xgoSzXgUPpc and theaudioprogrammer.com for how this works
  // auto g = apvts.getRawParameterValue("INGAIN");
  // buffer.applyGain(g->load());
}

juce::AudioProcessorEditor *J13AudioProcessor::createEditor()
{
  return new J13AudioProcessorEditor(*this);
}

//==============================================================================
void J13AudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void J13AudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
  return new J13AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout J13AudioProcessor::createParameters()
{
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  params.push_back(std::make_unique<juce::AudioParameterFloat>("INGAIN", "Gain", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTGAIN", "Output", 0.0f, 1.0f, 0.5f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWFREQ", "Low Freq", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWGAIN", "Low Gain", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWRESO", "Low Resonance", 0.0f, 1.0f, 0.5f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHFREQ", "High Freq", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHGAIN", "High Gain", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHRESO", "High Resonance", 0.0f, 1.0f, 0.5f));

  return {params.begin(), params.end()};
}

void J13AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  // DBG("in prepare");
  mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
                                      getMainBusNumOutputChannels(),
                                      sampleRate, samplesPerBlock);
  mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

  initialiseGraph();
}

void J13AudioProcessor::initialiseGraph()
{
  mainProcessor->clear();

  audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
  audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
  midiInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
  midiOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

  inputGainNode = mainProcessor->addNode(std::make_unique<GainProcessor>());
  highPassNode = mainProcessor->addNode(std::make_unique<HighPassFilterProcessor>());

  connectAudioNodes();
  connectMidiNodes();
}

void J13AudioProcessor::updateGraph()
{
}

void J13AudioProcessor::connectAudioNodes()
{
  for (int channel = 0; channel < 2; ++channel)
  {
    mainProcessor->addConnection({{audioInputNode->nodeID, channel},
                                  {inputGainNode->nodeID, channel}});

    mainProcessor->addConnection({{inputGainNode->nodeID, channel},
                                  {highPassNode->nodeID, channel}});

    mainProcessor->addConnection({{highPassNode->nodeID, channel},
                                  {audioOutputNode->nodeID, channel}});
  }
}

void J13AudioProcessor::connectMidiNodes()
{
  mainProcessor->addConnection({{midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                {midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}});
}
