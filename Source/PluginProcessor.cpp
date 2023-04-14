/*
  ==============================================================================

    PluginProcessor.cpp
    Created: Jan 2023
    Author:  jkokosa

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "Filters.h"
#include "GainProcessor.h"
#include "PluginEditor.h"
#include "Saturation.h"


J13AudioProcessor::J13AudioProcessor()
	: AudioProcessor(BusesProperties()
						 .withInput("Input", juce::AudioChannelSet::stereo(), true)
						 .withOutput("Output", juce::AudioChannelSet::stereo(), true))
	, apvts(*this, nullptr, "Parameters", createParameters())
	, mainProcessor(new juce::AudioProcessorGraph())
{
}

J13AudioProcessor::~J13AudioProcessor() { }

bool J13AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	// This is the place where you check if the layout is supported.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;

	return true;
}

void J13AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;

	for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	updateGraph();

	mainProcessor->processBlock(buffer, midiMessages);
}

juce::AudioProcessorEditor* J13AudioProcessor::createEditor() { return new J13AudioProcessorEditor(*this); }

//==============================================================================
void J13AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	auto state = apvts.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	// std::cout << "state = " << xml.get()->toString() << std::endl;

	// use this to remove obsolete parameters
	// auto ele = xml->getChildByAttribute("id", "LOWRESO");
	// if (ele == nullptr) {
	// 	std::cout << "got nullptr" << std::endl;
	// } else {
	// 	xml->removeChildElement(ele, true);
	// }

	//std::cout << xml.get()->toString() << std::endl;
	// for (auto* element : xml->getChildIterator()) {
	// 	std::cout << element->toString() << " ";
	// 	std::cout << element->getAttributeName(0) << " ";
	// 	std::cout << element->getAttributeName(1) << std::endl;
	// }

	//TODO: normalize all float parameters to some set number of digits so
	// we can use a stashed string to see if any paramets have changed and
	// a repaint of the plot is needed.
	copyXmlToBinary(*xml, destData);
}

void J13AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory
	// block, whose contents will have been created by the getStateInformation()
	// call.
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(apvts.state.getType()))
			apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new J13AudioProcessor(); }

juce::AudioProcessorValueTreeState::ParameterLayout J13AudioProcessor::createParameters()
{
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

	params.push_back(std::make_unique<juce::AudioParameterFloat>("INGAIN", "Gain", -12.0f, 12.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", -12.0f, 12.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterBool>("INCLEAN", "Input Clean", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("INWARM", "Input Warm", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("INBRIGHT", "Input Bright", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTGAIN", "Output", -12.0f, 12.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterBool>("OUTCLEAN", "Output Clean", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("OUTWARM", "Output Warm", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("OUTTHICK", "Output Thick", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWFREQ", "Low Freq", 20.0f, 220.0f, 100.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWGAIN", "Low Gain", -20.0f, 20.0f, 0.0f));

	params.push_back(std::make_unique<juce::AudioParameterBool>("LOWBUMP", "Low Bump", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("LOWSHELF", "Low Normal", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("LOWWIDE", "Low Wide", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWMIDFREQ", "Low Mid Freq", 220.0f, 3600.0f, 400.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWMIDGAIN", "Low Mid Gain", -20.0f, 20.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWMIDQ", "Low Mid Resonance", 0.2f, 4.0f, 0.7f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHMIDFREQ", "High Mid Freq", 1000.0f, 6000.0f, 2000.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHMIDGAIN", "High Mid Gain", -20.0f, 20.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHMIDQ", "High Mid Resonance", 0.2f, 4.0f, 0.7f));

	params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHFREQ", "High Freq", 4000.0f, 20000.0f, 4000.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHGAIN", "High Gain", -20.0f, 20.0f, 0.0f));

	params.push_back(std::make_unique<juce::AudioParameterBool>("HIGHBUMP", "High Bump", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("HIGHSHELF", "High Normal", false));
	params.push_back(std::make_unique<juce::AudioParameterBool>("HIGHWIDE", "High Wide", false));

	params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHPASS", "High Pass", 20.0f, 250.0f, 20.0f));

	return { params.begin(), params.end() };
}

void J13AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	J13AudioProcessor::sampleRateX = sampleRate;

	mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(), getMainBusNumOutputChannels(), sampleRate, samplesPerBlock);
	mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);

	initialiseGraph();

	smoothInGain.reset(sampleRate, 0.25f);
	smoothDrive.reset(sampleRate, 0.25f);
	smoothOutGain.reset(sampleRate, 0.25f);

	smoothLowFreq.reset(sampleRate, 0.25f);
	smoothLowGain.reset(sampleRate, 0.25f);
	smoothLowQ.reset(sampleRate, 0.25f);

	smoothLowMidFreq.reset(sampleRate, 0.25f);
	smoothLowMidGain.reset(sampleRate, 0.25f);
	smoothLowMidQ.reset(sampleRate, 0.25f);

	smoothHighMidFreq.reset(sampleRate, 0.25f);
	smoothHighMidGain.reset(sampleRate, 0.25f);
	smoothHighMidQ.reset(sampleRate, 0.25f);

	smoothHighFreq.reset(sampleRate, 0.25f);
	smoothHighGain.reset(sampleRate, 0.25f);
	smoothHighQ.reset(sampleRate, 0.25f);
}

void J13AudioProcessor::initialiseGraph()
{
	mainProcessor->clear();

	audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
	audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
	midiInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiInputNode));
	midiOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::midiOutputNode));

	inputGainNode = mainProcessor->addNode(std::make_unique<GainProcessor>());
	outputGainNode = mainProcessor->addNode(std::make_unique<GainProcessor>());
	driveNode = mainProcessor->addNode(std::make_unique<GainProcessor>());
	driveOffsetNode = mainProcessor->addNode(std::make_unique<GainProcessor>());

	inSaturationNode = mainProcessor->addNode(std::make_unique<SaturationProcessor>());
	outSaturationNode = mainProcessor->addNode(std::make_unique<SaturationProcessor>());

	highShelfNode = mainProcessor->addNode(std::make_unique<HighShelfProcessor>());
	highMidPeakNode = mainProcessor->addNode(std::make_unique<PeakProcessor>());
	lowMidPeakNode = mainProcessor->addNode(std::make_unique<PeakProcessor>());
	lowShelfNode = mainProcessor->addNode(std::make_unique<LowShelfProcessor>());
	highPassNode = mainProcessor->addNode(std::make_unique<HighPassProcessor>());

	connectAudioNodes();
	connectMidiNodes();
}

void J13AudioProcessor::updateGain(juce::StringRef ParameterID, juce::SmoothedValue<float>* smoother, Node::Ptr node)
{
	auto new_value = (apvts.getRawParameterValue(ParameterID))->load();

	smoother->setTargetValue(new_value);

	((GainProcessor*)node.get()->getProcessor())->updateGain(smoother->getNextValue());
}

void J13AudioProcessor::updateGraph()
{
	// see https://www.youtube.com/watch?v=xgoSzXgUPpc and theaudioprogrammer.com
	// for how this works
	//-------------------------------------------------------------

	auto skipSize = getBlockSize() - 1;

	updateGain("INGAIN", &smoothInGain, inputGainNode);
	smoothInGain.skip(skipSize);

	updateGain("DRIVE", &smoothDrive, driveNode);
	((GainProcessor*)driveOffsetNode.get()->getProcessor())->updateGain(2.0f - smoothDrive.getCurrentValue());
	smoothDrive.skip(skipSize);

	updateGain("OUTGAIN", &smoothOutGain, outputGainNode);
	smoothOutGain.skip(skipSize);

	//-------------------------------------------------------------
	auto inClean = (apvts.getRawParameterValue("INCLEAN"))->load();
	auto inWarm = (apvts.getRawParameterValue("INWARM"))->load();
	// auto inBright = (apvts.getRawParameterValue("INBRIGHT"))->load();

	if (inClean) {
		((SaturationProcessor*)inSaturationNode.get()->getProcessor())->setSaturationType(SaturationProcessor::clean);
	} else if (inWarm) {
		((SaturationProcessor*)inSaturationNode.get()->getProcessor())->setSaturationType(SaturationProcessor::warm);
	} else {
		((SaturationProcessor*)inSaturationNode.get()->getProcessor())->setSaturationType(SaturationProcessor::bright);
	}

	//-------------------------------------------------------------
	auto outClean = (apvts.getRawParameterValue("OUTCLEAN"))->load();
	auto outWarm = (apvts.getRawParameterValue("OUTWARM"))->load();
	// auto outThick = (apvts.getRawParameterValue("OUTTHICK"))->load();

	if (outClean) {
		((SaturationProcessor*)outSaturationNode.get()->getProcessor())->setSaturationType(SaturationProcessor::clean);
	} else if (outWarm) {
		((SaturationProcessor*)outSaturationNode.get()->getProcessor())->setSaturationType(SaturationProcessor::warm);
	} else {
		((SaturationProcessor*)outSaturationNode.get()->getProcessor())->setSaturationType(SaturationProcessor::thick);
	}

	//-------------------------------------------------------------
	auto lowfreq = (apvts.getRawParameterValue("LOWFREQ"))->load();
	smoothLowFreq.setTargetValue(lowfreq);

	auto lowgainDb = (apvts.getRawParameterValue("LOWGAIN"))->load();
	auto lowgain = juce::Decibels::decibelsToGain(lowgainDb);

	if (lowgain < 0.1f) {
		lowgain = 0.1f;
	}

	smoothLowGain.setTargetValue(lowgain);

	// auto lowShelf = (apvts.getRawParameterValue("LOWSHELF"))->load();
	auto lowBump = (apvts.getRawParameterValue("LOWBUMP"))->load();
	auto lowWide = (apvts.getRawParameterValue("LOWWIDE"))->load();

	float lowQ;

	if (lowBump) {
		if (smoothLowGain.getCurrentValue() > 1.0f) {
			lowQ = 1.1f;
		} else {
			lowQ = 1.4f;
		}
	} else if (lowWide) {
		lowQ = 0.4f;
	} else {
		lowQ = 0.7f;
	}

	smoothLowQ.setTargetValue(lowQ);

	((LowShelfProcessor*)lowShelfNode.get()->getProcessor())
		->updateSettings(sampleRateX, smoothLowFreq.getNextValue(), smoothLowQ.getNextValue(), smoothLowGain.getNextValue());

	smoothLowFreq.skip(getBlockSize() - 1);
	smoothLowQ.skip(getBlockSize() - 1);
	smoothLowGain.skip(getBlockSize() - 1);

	//-------------------------------------------------------------
	//-------------------------------------------------------------
	auto lowMidFreq = (apvts.getRawParameterValue("LOWMIDFREQ"))->load();
	smoothLowMidFreq.setTargetValue(lowMidFreq);

	auto lowMidQ = (apvts.getRawParameterValue("LOWMIDQ"))->load();
	if (lowMidQ < 0.1f) {
		lowMidQ = 0.1f;
	}
	smoothLowMidQ.setTargetValue(lowMidQ);

	auto lowMidGainDb = (apvts.getRawParameterValue("LOWMIDGAIN"))->load();
	auto lowMidGain = juce::Decibels::decibelsToGain(lowMidGainDb);

	if (lowMidGain < 0.1f) {
		lowMidGain = 0.1f;
	}

	smoothLowMidGain.setTargetValue(lowMidGain);

	((PeakProcessor*)lowMidPeakNode.get()->getProcessor())
		->updateSettings(
			sampleRateX, smoothLowMidFreq.getNextValue(), smoothLowMidQ.getNextValue(), smoothLowMidGain.getNextValue());

	smoothLowMidFreq.skip(getBlockSize() - 1);
	smoothLowMidQ.skip(getBlockSize() - 1);
	smoothLowMidGain.skip(getBlockSize() - 1);

	//-------------------------------------------------------------
	//-------------------------------------------------------------
	auto highMidFreq = (apvts.getRawParameterValue("HIGHMIDFREQ"))->load();
	smoothHighMidFreq.setTargetValue(highMidFreq);

	auto highMidQ = (apvts.getRawParameterValue("HIGHMIDQ"))->load();
	if (highMidQ < 0.1f) {
		highMidQ = 0.1f;
	}
	smoothHighMidQ.setTargetValue(highMidQ);

	auto highMidGainDb = (apvts.getRawParameterValue("HIGHMIDGAIN"))->load();
	auto highMidGain = juce::Decibels::decibelsToGain(highMidGainDb);

	if (highMidGain < 0.1f) {
		highMidGain = 0.1f;
	}

	smoothHighMidGain.setTargetValue(highMidGain);

	((PeakProcessor*)highMidPeakNode.get()->getProcessor())
		->updateSettings(
			sampleRateX, smoothHighMidFreq.getNextValue(), smoothHighMidQ.getNextValue(), smoothHighMidGain.getNextValue());

	smoothHighMidFreq.skip(getBlockSize() - 1);
	smoothHighMidQ.skip(getBlockSize() - 1);
	smoothHighMidGain.skip(getBlockSize() - 1);

	//-------------------------------------------------------------
	//-------------------------------------------------------------
	auto highfreq = (apvts.getRawParameterValue("HIGHFREQ"))->load();
	smoothHighFreq.setTargetValue(highfreq);

	// auto highShelf = (apvts.getRawParameterValue("HIGHSHELF"))->load();
	auto highBump = (apvts.getRawParameterValue("HIGHBUMP"))->load();
	auto highWide = (apvts.getRawParameterValue("HIGHWIDE"))->load();

	float highQ = 0.7f;

	if (highBump) {
		highQ = 1.4f;
	} else if (highWide) {
		highQ = 0.4f;
	}

	smoothHighQ.setTargetValue(highQ);

	auto highgainDb = (apvts.getRawParameterValue("HIGHGAIN"))->load();
	auto highgain = juce::Decibels::decibelsToGain(highgainDb);

	if (highgain < 0.1f) {
		highgain = 0.1f;
	}

	smoothHighGain.setTargetValue(highgain);

	((HighShelfProcessor*)highShelfNode.get()->getProcessor())
		->updateSettings(sampleRateX, smoothHighFreq.getNextValue(), smoothHighQ.getNextValue(), smoothHighGain.getNextValue());

	smoothHighFreq.skip(getBlockSize() - 1);
	smoothHighQ.skip(getBlockSize() - 1);
	smoothHighGain.skip(getBlockSize() - 1);


	// auto x = ((HighShelfProcessor*)highShelfNode.get()->getProcessor());
	// auto y = x->getBypassParameter();

	//-------------------------------------------------------------
	//-------------------------------------------------------------
	auto highPassFreq = (apvts.getRawParameterValue("HIGHPASS"))->load();
	smoothHighPass.setTargetValue(highPassFreq);

	((HighPassProcessor*)highPassNode.get()->getProcessor())->updateSettings(sampleRateX, smoothHighPass.getNextValue());

	smoothHighPass.skip(getBlockSize() - 1);
}

juce::dsp::IIR::Coefficients<float>* J13AudioProcessor::getCoeffs(int filterNum)
{
	juce::AudioProcessorGraph::Node* node;
	if (filterNum == 0) {
		node = lowShelfNode.get();
	} else if (filterNum == 1) {
		node = lowMidPeakNode.get();
	} else if (filterNum == 2) {
		node = highMidPeakNode.get();
	} else if (filterNum == 3) {
		node = highShelfNode.get();
	} else if (filterNum == 4) {
		node = highPassNode.get();
	} else {
		return nullptr;
	}

	if (node == nullptr) {
		return nullptr;
	}

	auto proc = node->getProcessor();

	if (filterNum == 0) {
		return ((LowShelfProcessor*)proc)->getCoeffs();
	} else if (filterNum == 1) {
		return ((PeakProcessor*)proc)->getCoeffs();
	} else if (filterNum == 2) {
		return ((PeakProcessor*)proc)->getCoeffs();
	} else if (filterNum == 3) {
		return ((HighShelfProcessor*)proc)->getCoeffs();
	} else if (filterNum == 4) {
		return ((HighPassProcessor*)proc)->getCoeffs();
	}

	return nullptr;
}

void J13AudioProcessor::connectAudioNodes()
{
	for (int channel = 0; channel < 2; ++channel) {
		mainProcessor->addConnection({ { audioInputNode->nodeID, channel }, { inputGainNode->nodeID, channel } });

		mainProcessor->addConnection({ { inputGainNode->nodeID, channel }, { inSaturationNode->nodeID, channel } });

		mainProcessor->addConnection({ { inSaturationNode->nodeID, channel }, { lowShelfNode->nodeID, channel } });

		mainProcessor->addConnection({ { lowShelfNode->nodeID, channel }, { lowMidPeakNode->nodeID, channel } });

		mainProcessor->addConnection({ { lowMidPeakNode->nodeID, channel }, { driveNode->nodeID, channel } });

		mainProcessor->addConnection({ { driveNode->nodeID, channel }, { highMidPeakNode->nodeID, channel } });

		mainProcessor->addConnection({ { highMidPeakNode->nodeID, channel }, { highShelfNode->nodeID, channel } });

		mainProcessor->addConnection({ { highShelfNode->nodeID, channel }, { outSaturationNode->nodeID, channel } });

		mainProcessor->addConnection({ { outSaturationNode->nodeID, channel }, { outputGainNode->nodeID, channel } });

		mainProcessor->addConnection({ { outputGainNode->nodeID, channel }, { driveOffsetNode->nodeID, channel } });

		mainProcessor->addConnection({ { driveOffsetNode->nodeID, channel }, { highPassNode->nodeID, channel } });

		mainProcessor->addConnection({ { highPassNode->nodeID, channel }, { audioOutputNode->nodeID, channel } });
	}
}

void J13AudioProcessor::connectMidiNodes()
{
	mainProcessor->addConnection({ { midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex },
		{ midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
}