#pragma once

#include <JuceHeader.h>


const auto JadeWhite(Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.0));
const auto JadeRed(Colour::fromFloatRGBA(0.890196078431373, 0.023529411764706, 0.074509803921569, 1.0));
const auto JadeTeal(Colour::fromFloatRGBA(0.352941176470588, 0.372549019607843, 0.337254901960784, 1.0));
const auto JadeGray(Colour::fromFloatRGBA(0.356862745098039, 0.372549019607843, 0.341176470588235, 1.0));

const auto JadeLightRed1(Colour::fromFloatRGBA(0.956862745098039, 0.631372549019608, 0.552941176470588, 1.0));
const auto JadeLightRed2(Colour::fromFloatRGBA(0.968627450980392, 0.733333333333333, 0.682352941176471, 1.0));
const auto JadeLightRed3(Colour::fromFloatRGBA(0.980392156862745, 0.807843137254902, 0.776470588235294, 1.0));
// JadeLightRed4 = [0.988235294117647 0.890196078431373 0.870588235294118];

class JadeLookAndFeel : public LookAndFeel_V4
{
public:
	JadeLookAndFeel();

private:
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
};

