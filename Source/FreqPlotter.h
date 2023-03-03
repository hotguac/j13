/*
  ==============================================================================

    FreqPlotter.h
    Created: 28 Feb 2023 8:26:13am
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class FreqPlotter : public juce::Component {
public:
	FreqPlotter()
	{
		setfrequencyRange(20.0f, 20000.0f);
		setMagnitudeMax(20.0f);
	}

	~FreqPlotter() override { }

	void paint(juce::Graphics& g) override
	{
		renderGraph(g);
		return;
	}

	void resized() override
	{
		// Set the bounds of any child components that your component contains..
		juce::Rectangle<float> area(getWidth(), getHeight());

		setArea(area);

		auto width = area.getWidth();
		auto height = area.getHeight();

		canvas = juce::Image(juce::Image::PixelFormat::RGB, width, height, true);
		juce::Graphics g(canvas);

		backgroundImage = juce::Image(juce::Image::PixelFormat::RGB, width, height, true);
		juce::Graphics gg(backgroundImage);
		gg.fillAll(juce::Colours::lightgrey);

		drawBackground(gg);
	}

	void setSampleRate(float rate) { sampleRate = rate; }

	void addCoeffs(juce::dsp::IIR::Coefficients<float>::Ptr coeff) { curveCoeffs.add(coeff); }

	void clearCoeffs()
	{
		if (curveCoeffs.size() > 0) {
			curveCoeffs.clear();
		}
	}

	void renderGraph(juce::Graphics& g)
	{
		g.drawImageAt(backgroundImage, 0, 0, false);

		auto n = curveCoeffs.size();

		setPlotColour(juce::Colours::darkred);
		drawCompositeCurve(g);

		setPlotColour(juce::Colours::darkgreen);

		for (int i = 0; i < n; ++i) {
			drawCurve(g, i);
		}
	}

private:
	juce::Image backgroundImage;
	juce::Image canvas;

	juce::Rectangle<float> graphArea { 0.0f, 0.0f, 0.0f, 0.0f };
	juce::Rectangle<float> frequencyLabelArea { 0.0f, 0.0f, 0.0f, 0.0f };
	juce::Rectangle<float> magnitudeLabelArea { 0.0f, 0.0f, 0.0f, 0.0f };

	float fontExtraSpace = 6.0f;
	juce::Font font { 10.0f };
	float fontWidth = font.getStringWidthFloat("-100 dB");
	float fontHeight = font.getHeight();

	juce::Colour backgroundColour { juce::Colours::whitesmoke };
	juce::Colour gridColour { juce::Colours::darkgrey };
	juce::Colour plotColour { juce::Colours::darkgreen };
	juce::Colour labelAreaColour { juce::Colours::green };
	juce::Colour labelFontColour { juce::Colours::whitesmoke };

	float areaX = 0.0f;
	float areaY = 0.0f;
	float areaHeight = 0.0f;
	float areaWidth = 0.0f;

	float magnitudeX = 0.0f;
	float magnitudeY = 0.0f;
	float magHeight = 0.0f;
	float magnitudeWidth = 0.0f;

	float frequencyX = 0.0f;
	float frequencyY = 0.0f;
	float frequencyHeight = 0.0f;
	float frequencyWidth = 0.0f;

	float graphX = 0.0f;
	float graphY = 0.0f;
	float graphHeight = 0.0f;
	float graphWidth = 0.0f;

	float maxGain = 10.0f;
	float minGain = 0.1f;

	float minFrequency = 20.0f;
	float maxFrequency = 20000.0f;

	float sampleRate = 48000.0f;

	juce::Array<juce::dsp::IIR::Coefficients<float>::Ptr> curveCoeffs;

	float gainToGraphY(float gain)
	{
		if (gain > maxGain)
			gain = maxGain;

		if (gain < minGain)
			gain = minGain;

		auto maxDecibels = juce::Decibels::gainToDecibels(maxGain);
		auto minDecibels = juce::Decibels::gainToDecibels(minGain);
		auto decibels = juce::Decibels::gainToDecibels(gain);

		auto decibelRange = maxDecibels - minDecibels;
		auto yRange = graphHeight;

		auto py = ((maxDecibels - decibels) / decibelRange) * yRange;

		return py;
	}

	float frequencyToGraphX(float frequency)
	{
		auto logMin = log10(minFrequency / minFrequency); //  ==>> graphX
		auto logMax = log10(maxFrequency / minFrequency); //  ==>> graphX + graphWidth
		auto logFreq = log10(frequency / minFrequency);

		auto xMin = graphX;
		auto xMax = graphX + graphWidth;

		auto xScaleFactor = (xMax - xMin) / logMax;
		auto px = graphX + (xScaleFactor * log10(frequency / minFrequency));

		return px;
	}

	juce::Point<float> getGraphPoint(float frequency, float gain)
	{
		auto px = frequencyToGraphX(frequency);
		auto py = gainToGraphY(gain);

		return (juce::Point<float>(px, py));
	}

	float getGraphFreq(float x)
	{
		auto logMax = log10(maxFrequency / minFrequency);

		auto xMin = graphX;
		auto xMax = graphX + graphWidth;

		auto xScaleFactor = (xMax - xMin) / logMax;

		auto u = (x - graphX) / xScaleFactor;
		float frequency = pow(10.0, u) * minFrequency;

		return frequency;
	}

	juce::String getFreqString(float frequency)
	{
		if (frequency < 1000.0f) {
			return juce::String(frequency);
		} else {
			if (frequency < 10000.0f) {
				juce::String temp("");
				auto result = juce::String(frequency / 1000.0f, 1);
				temp += result.getCharPointer()[0];
				temp += 'K';
				return temp;
			} else {
				juce::String temp("");
				auto result = juce::String(frequency / 1000.0f, 1);
				temp += result.getCharPointer()[0];
				temp += result.getCharPointer()[1];
				temp += 'K';
				return temp;
			}
		}
	}

	void drawFreqLabel(juce::Graphics& g, float frequency)
	{
		const juce::String& text(getFreqString(frequency));

		juce::Point<float> top { frequencyToGraphX(frequency), frequencyLabelArea.getY() };
		top.addXY(-(fontWidth / 2.0f), 2.0f);

		auto bot = top;
		bot.addXY(fontWidth + 6, fontHeight + 4);

		juce::Rectangle<float> mark(top, bot);

		juce::Rectangle<int> area(mark.getX(), mark.getY(), mark.getWidth(), mark.getHeight());
		juce::Justification justification(juce::Justification::centred);
		const int maximumNumberOfLines = 2;
		const float minimumHorizontalScale = 0.0f;

		g.setColour(labelFontColour);
		g.drawFittedText(text, area, justification, maximumNumberOfLines, minimumHorizontalScale);
	}

	void drawDecibelLabel(juce::Graphics& g, float decibels)
	{
		int d = (int)decibels;
		const juce::String& text(std::to_string(d));

		auto gain = juce::Decibels::decibelsToGain(decibels);
		juce::Point<float> top { magnitudeLabelArea.getX(), gainToGraphY(gain) };
		top.addXY(2.0f, -(fontHeight / 2.1f));

		auto bot = top;
		float fontWidth = font.getStringWidthFloat("-20dB");
		bot.addXY(fontWidth + 6, fontHeight + 4);

		juce::Rectangle<float> mark(top, bot);

		juce::Rectangle<int> area(mark.getX(), mark.getY(), mark.getWidth(), mark.getHeight());
		juce::Justification justification(juce::Justification::topRight);
		const int maximumNumberOfLines = 1;
		const float minimumHorizontalScale = 0.0f;

		g.setColour(labelFontColour);
		g.drawFittedText(text, area, justification, maximumNumberOfLines, minimumHorizontalScale);
	}

	void drawFreqAxis(juce::Graphics& g, float frequency, const float* dashLengths, int numDashLengths)
	{
		auto yTop = graphY;
		auto yBottom = graphY + graphHeight;
		auto x = frequencyToGraphX(frequency);

		g.setColour(gridColour);
		g.drawDashedLine(juce::Line<float>(x, yTop, x, yBottom), dashLengths, numDashLengths);
		drawFreqLabel(g, frequency);
	}

	void drawDecibelAxis(juce::Graphics& g, float decibels, const float* dashLengths, int numDashLengths)
	{
		auto gain = juce::Decibels::decibelsToGain(decibels);
		auto y = gainToGraphY(gain);

		g.setColour(gridColour);
		g.drawDashedLine(juce::Line<float>(graphX, y, graphX + graphWidth, y), dashLengths, numDashLengths);
		drawDecibelLabel(g, decibels);
	}

	void setBackground(juce::Colour colour) { backgroundColour = colour; }

	void setGridColour(juce::Colour colour) { gridColour = colour; }

	void setPlotColour(juce::Colour colour) { plotColour = colour; }

	void drawBackground(juce::Graphics& g)
	{
		g.setColour(backgroundColour);
		g.fillRect(graphArea);

		g.setColour(labelAreaColour);
		g.fillRect(magnitudeLabelArea);
		g.fillRect(frequencyLabelArea);

		g.setColour(gridColour);

		//---------
		float dashLengths[2] = { 48.0f, 1.0f };
		drawFreqAxis(g, 30.0f, dashLengths, 2);
		drawFreqAxis(g, 60.0f, dashLengths, 2);
		drawFreqAxis(g, 100.0f, dashLengths, 2);
		drawFreqAxis(g, 160.0f, dashLengths, 2);
		drawFreqAxis(g, 300.0f, dashLengths, 2);
		drawFreqAxis(g, 600.0f, dashLengths, 2);
		drawFreqAxis(g, 1000.0f, dashLengths, 2);
		drawFreqAxis(g, 2000.0f, dashLengths, 2);
		drawFreqAxis(g, 4000.0f, dashLengths, 2);
		drawFreqAxis(g, 8000.0f, dashLengths, 2);
		drawFreqAxis(g, 16000.0f, dashLengths, 2);

		dashLengths[0] = 48.0f;
		dashLengths[1] = 1.0f;

		drawDecibelAxis(g, 0.0f, dashLengths, 2);

		dashLengths[0] = 4.0f;
		dashLengths[1] = 4.0f;

		drawDecibelAxis(g, -10.0f, dashLengths, 2);
		drawDecibelAxis(g, 10.0f, dashLengths, 2);

		dashLengths[0] = 2.0f;
		dashLengths[1] = 6.0f;

		drawDecibelAxis(g, -15.0f, dashLengths, 2);
		drawDecibelAxis(g, -5.0f, dashLengths, 2);
		drawDecibelAxis(g, 5.0f, dashLengths, 2);
		drawDecibelAxis(g, 15.0f, dashLengths, 2);
	}

	void drawCurve(juce::Graphics& g, int curveNum)
	{
		auto lastX = -1.0f;
		auto lastY = -1.0f;

		g.setColour(plotColour);

		if (curveNum == 1) {
			g.setColour(plotColour.withHue(0.8));
		}

		auto z = curveCoeffs[curveNum].get();

		auto minX = frequencyToGraphX(minFrequency);
		auto maxX = frequencyToGraphX(maxFrequency);

		for (auto x = minX; x <= maxX; ++x) {
			auto freq = getGraphFreq(x);

			auto magnitude = z->getMagnitudeForFrequency(freq, sampleRate);
			auto p = getGraphPoint(freq, magnitude);

			if (lastX != -1.0f) {
				g.drawLine(lastX, lastY, p.getX(), p.getY(), 2.0f);
			}

			lastX = p.getX();
			lastY = p.getY();
		}
	}

	void drawCompositeCurve(juce::Graphics& g)
	{
		auto lastX = -1.0f;
		auto lastY = -1.0f;

		auto numCurves = curveCoeffs.size();
		float magnitude;

		if (numCurves < 1) {
			return;
		}

		auto z = curveCoeffs[0].get();

		auto minX = frequencyToGraphX(minFrequency);
		auto maxX = frequencyToGraphX(maxFrequency);

		g.setColour(plotColour);

		for (auto x = minX; x <= maxX; ++x) {
			auto freq = getGraphFreq(x);

			magnitude = 0.0f;
			for (auto curveNum = 0; curveNum < numCurves; ++curveNum) {
				z = curveCoeffs[curveNum].get();
				// normalize magnitude by subtracting one here
				// to prevent the curve shifting
				magnitude += z->getMagnitudeForFrequency(freq, sampleRate) - 1.0f;
			}

			auto p = getGraphPoint(freq, magnitude + 1.0f); // add the one back in here

			if (lastX != -1.0f) {
				g.setColour(juce::Colours::blue);
				g.drawLine(lastX, lastY, p.getX(), p.getY(), 2.0f);

				auto height = p.getY() - (graphHeight / 2);
				auto y = graphY + (graphHeight / 2);

				if (magnitude > 0.0f) {
					height *= -1.0f;
					y = y - height;
				}

				juce::Colour color = juce::Colours::lightblue.withAlpha(0.2f);
				g.setColour(color);

				juce::Rectangle<int> fillArea(p.getX(), y, 1, height);
				g.fillRect(fillArea);
			}

			lastX = p.getX();
			lastY = p.getY();
		}
	}

	void setfrequencyRange(float min, float max)
	{
		minFrequency = min;
		maxFrequency = max;
	}

	void setFont(juce::Font newFont) { font = newFont; }

	void setMagnitudeMax(float max) { }

	void setArea(juce::Rectangle<float> area)
	{
		fontWidth = font.getStringWidthFloat("-100 dB");
		fontHeight = font.getHeight();

		areaX = area.getPosition().getX();
		areaY = area.getPosition().getY();
		areaHeight = area.getHeight();
		areaWidth = area.getWidth();

		magnitudeX = areaX;
		magnitudeY = areaY;
		magHeight = areaHeight - (fontHeight + fontExtraSpace);
		magnitudeWidth = fontWidth + fontExtraSpace;

		magnitudeLabelArea = juce::Rectangle<float>(magnitudeX, magnitudeY, magnitudeWidth, magHeight);

		frequencyX = areaX;
		frequencyY = areaY + areaHeight - (fontHeight + fontExtraSpace);
		frequencyHeight = (fontHeight + fontExtraSpace);
		frequencyWidth = areaWidth;

		frequencyLabelArea = juce::Rectangle<float>(frequencyX, frequencyY, frequencyWidth, frequencyHeight);

		graphX = areaX + magnitudeWidth;
		graphY = areaY;
		graphHeight = areaHeight - frequencyHeight;
		graphWidth = areaWidth - magnitudeWidth;

		graphArea = juce::Rectangle<float>(graphX, graphY, graphWidth, graphHeight);
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreqPlotter)
};
