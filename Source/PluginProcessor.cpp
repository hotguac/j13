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

  params.push_back(std::make_unique<juce::AudioParameterFloat>("INGAIN", "Gain", -24.0f, 20.0f, 0.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", -24.0f, 20.0f, 0.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTGAIN", "Output", -24.0f, 20.0f, 0.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWFREQ", "Low Freq", 30.0f, 1000.0f, 1.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWGAIN", "Low Gain", 0.01f, 2.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWQ", "Low Resonance", 0.01f, 3.0f, 0.5f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHFREQ", "High Freq", 1000.0f, 12000.0f, 2000.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHGAIN", "High Gain", 0.01f, 2.0f, 1.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHQ", "High Resonance", 0.01f, 3.0f, 0.5f));

  return {params.begin(), params.end()};
}

void J13AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  J13AudioProcessor::sampleRate = sampleRate;
  // DBG("in prepare");
  mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
                                      getMainBusNumOutputChannels(),
                                      sampleRate, samplesPerBlock);
  mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

  initialiseGraph();

  smoothInGain.reset(sampleRate, 0.5f);
  smoothDrive.reset(sampleRate, 0.5f);
  smoothOutGain.reset(sampleRate, 0.5f);

  smoothLowFreq.reset(sampleRate, 0.5f);
  smoothLowGain.reset(sampleRate, 0.5f);
  smoothLowQ.reset(sampleRate, 0.5f);

  smoothHighFreq.reset(sampleRate, 0.5f);
  smoothHighGain.reset(sampleRate, 0.5f);
  smoothHighQ.reset(sampleRate, 0.5f);
}

void J13AudioProcessor::initialiseGraph()
{
  mainProcessor->clear();

  audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
  audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
  midiInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
  midiOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

  inputGainNode = mainProcessor->addNode(std::make_unique<GainProcessor>());
  highShelfNode = mainProcessor->addNode(std::make_unique<HighShelfProcessor>());
  lowShelfNode = mainProcessor->addNode(std::make_unique<LowShelfProcessor>());

  connectAudioNodes();
  connectMidiNodes();
}

void J13AudioProcessor::updateGraph()
{
  // see https://www.youtube.com/watch?v=xgoSzXgUPpc and theaudioprogrammer.com for how this works
  auto gi = apvts.getRawParameterValue("INGAIN");
  smoothInGain.setTargetValue(gi->load());

  ((GainProcessor *)inputGainNode.get()->getProcessor())->updateGain(smoothInGain.getNextValue());
  smoothInGain.skip(getBlockSize() - 1);

  //-------------------------------------------------------------
  auto lowfreq = (apvts.getRawParameterValue("LOWFREQ"))->load();
  smoothLowFreq.setTargetValue(lowfreq);

  auto lowq = (apvts.getRawParameterValue("LOWQ"))->load();
  if (lowq < 0.1f)
  {
    lowq = 0.1f;
  }
  smoothLowQ.setTargetValue(lowq);

  auto lowgain = (apvts.getRawParameterValue("LOWGAIN"))->load();
  if (lowgain < 0.1f)
  {
    lowgain = 0.1f;
  }

  smoothLowGain.setTargetValue(lowgain);

  ((LowShelfProcessor *)lowShelfNode.get()->getProcessor())->updateSettings(sampleRate, smoothLowFreq.getNextValue(), smoothLowQ.getNextValue(), smoothLowGain.getNextValue());

  smoothLowFreq.skip(getBlockSize() - 1);
  smoothLowQ.skip(getBlockSize() - 1);
  smoothLowGain.skip(getBlockSize() - 1);

  //-------------------------------------------------------------
  auto highfreq = (apvts.getRawParameterValue("HIGHFREQ"))->load();
  smoothHighFreq.setTargetValue(highfreq);

  auto highq = (apvts.getRawParameterValue("HIGHQ"))->load();
  if (highq < 0.1f)
  {
    highq = 0.1f;
  }
  smoothHighQ.setTargetValue(highq);

  auto highgain = (apvts.getRawParameterValue("HIGHGAIN"))->load();
  if (highgain < 0.1f)
  {
    highgain = 0.1f;
  }

  smoothHighGain.setTargetValue(highgain);

  ((HighShelfProcessor *)highShelfNode.get()->getProcessor())->updateSettings(sampleRate, smoothHighFreq.getNextValue(), smoothHighQ.getNextValue(), smoothHighGain.getNextValue());

  smoothHighFreq.skip(getBlockSize() - 1);
  smoothHighQ.skip(getBlockSize() - 1);
  smoothHighGain.skip(getBlockSize() - 1);
}

void J13AudioProcessor::connectAudioNodes()
{
  for (int channel = 0; channel < 2; ++channel)
  {
    mainProcessor->addConnection({{audioInputNode->nodeID, channel},
                                  {inputGainNode->nodeID, channel}});

    mainProcessor->addConnection({{inputGainNode->nodeID, channel},
                                  {lowShelfNode->nodeID, channel}});

    mainProcessor->addConnection({{lowShelfNode->nodeID, channel},
                                  {highShelfNode->nodeID, channel}});

    mainProcessor->addConnection({{highShelfNode->nodeID, channel},
                                  {audioOutputNode->nodeID, channel}});
  }
}

void J13AudioProcessor::connectMidiNodes()
{
  mainProcessor->addConnection({{midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                {midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}});
}
