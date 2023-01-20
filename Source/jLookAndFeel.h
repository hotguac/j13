/*
  ==============================================================================

    jLookAndFeel.h
    Created: 19 Jan 2023 10:30:35am
    Author:  jkokosa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define LABELFONTSIZE 18.0f

class jLookAndFeel : public juce::LookAndFeel_V4
{
public:
  juce::Colour fillColour = juce::Colours::darkred;
  juce::Colour outlineColour = juce::Colours::darkolivegreen;
  juce::Colour lineColour = juce::Colours::white;

  ~jLookAndFeel() override{};

  void drawRotarySlider(Graphics &g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        Slider &slider) override

  {
    float diameter = jmin(width, height) * 0.8f;
    float radius = diameter / 2;
    float centreX = x + width / 2;
    float centreY = y + height / 2;
    float rx = centreX - radius;
    float ry = centreY - radius;
    float rw = radius * 2.0f;
    float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

    // std::cout << rotaryStartAngle << " " << rotaryEndAngle << std::endl;

    // fill
    g.setColour(fillColour);
    g.fillEllipse(rx, ry, rw, rw);

    // outline
    g.setColour(outlineColour);
    g.drawEllipse(rx, ry, rw, rw, 3.0f);

    // set angle for path
    juce::Path p;
    float pointerLength = radius * 0.7f;
    float pointerThickness = 4.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // pointer
    g.setColour(lineColour);
    g.fillPath(p);
  }

  Label *createSliderTextBox(Slider &slider) override
  {
    juce::Label *l = LookAndFeel_V2::createSliderTextBox(slider);
    l->setFont(20.0f);

    return l;
  }
};
