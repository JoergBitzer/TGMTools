/**
 * @file JadeLookAndFeel.h
 * @author J. Bitzer at Jade Hochschule
 * @brief implments the look and feel for the Jade PlugINs (color schemes)
 * @version 0.1
 * @date 2021-01-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */
/* ToDo:
	1. Implement more custom GUIS 
*/

#pragma once

#include <JuceHeader.h>

/**
 * @brief define the Jade color scheme given by the CD 
 */
const auto JadeWhite(Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.0));
const auto JadeRed(Colour::fromFloatRGBA(0.890196078431373, 0.023529411764706, 0.074509803921569, 1.0));
const auto JadeTeal(Colour::fromFloatRGBA(0.352941176470588, 0.372549019607843, 0.337254901960784, 1.0));
const auto JadeGray(Colour::fromFloatRGBA(0.356862745098039, 0.372549019607843, 0.341176470588235, 1.0));

const auto JadeLightRed1(Colour::fromFloatRGBA(0.956862745098039, 0.631372549019608, 0.552941176470588, 1.0));
const auto JadeLightRed2(Colour::fromFloatRGBA(0.968627450980392, 0.733333333333333, 0.682352941176471, 1.0));
const auto JadeLightRed3(Colour::fromFloatRGBA(0.980392156862745, 0.807843137254902, 0.776470588235294, 1.0));
// JadeLightRed4 = [0.988235294117647 0.890196078431373 0.870588235294118];

/**
 * @brief the Look and Feel implements the rotary knob in a special way
 * 
 */
class JadeLookAndFeel : public LookAndFeel_V4
{
public:
	JadeLookAndFeel();

private:
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
};

