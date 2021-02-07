/*----------------------------------------------------------------------------------------
envelope class: Implements a typical synthesizer envelope
version: 0.01 first build
version 1.0 ADSR tested and works fine
author: Martin Berdau
(c) BSD 3-clause licence (https://opensource.org/licenses/BSD-3-Clause)
version 1.1 added parameter and GUI JB 06.2020 
version 1.2 level parameter JB 28.06.2020
-----------------------------------------------------------------------------------------*/

#pragma once

#include <vector>
#include <math.h>

class Envelope
{
public:
	enum envelopePhases
	{
		Off = 0,
		Delay,
		Attack,
		Hold,
		Decay,
		Sustain,
		Release,
	};
	Envelope();
	~Envelope();

	int getData(std::vector<double>& data);
	void NoteOn(); //to do: was passiert wenn noteon in einem Block?
	void NoteOff();
	void setSamplerate(double samplerate);
	void setDelayTime(double del_ms);
	void setAttackRate(double att_ms);
	void setHoldTime(double hold_ms);
	void setDecayRate(double dec_ms);
	void setSustainLevel(double level);
	void setReleaseRate(double rel_ms);
	void setMaxLevel(double level) { m_maxLevel = level; };
	void setInvertOnOff(bool onoff) {
		m_invertOn = onoff;
	};
	envelopePhases getEnvelopeStatus() { return m_envelopePhase; };

protected:
	void updateTimeConstants(void);
	bool m_invertOn;
	double m_fs;

	double m_delTime_ms;
	double m_delSamples;

	double m_tauAtt_ms;
	double m_alphaAtt;
	double m_attOver;

	double m_holdTime_ms;
	double m_holdSamples;

	double m_tauDec_ms;
	double m_alphaDec;

	double m_tauRel_ms;
	double m_alphaRel;

	double m_sustainLevel;

	envelopePhases m_envelopePhase;
	double m_envGain;

	int m_sampleCounter;
	double m_maxLevel;
};

#ifdef USE_JUCE
#include <JuceHeader.h>
// several IDs to allow more than one instance
#define MAX_ENV_INSTANCES 4
const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Delay", "Env2Delay", "Env3Delay", "Env4Delay" };
	std::string name = "EnvDelay";
	std::string unitName = "ms";
	float minValue = 0.f;
	float maxValue = 250.f;
	float defaultValue = 0.f;
}paramEnvDelay;

const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Hold", "Env2Hold", "Env3Hold", "Env4Hold" };
	std::string name = "EnvHold";
	std::string unitName = "ms";
	float minValue = 0.f;
	float maxValue = 250.f;
	float defaultValue = 0.f;
}paramEnvHold;

const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Sustain", "Env2Sustain", "Env3Sustain", "Env4Sustain" };
	std::string name = "EnvSustainLevel";
	std::string unitName = "";
	float minValue = 0.f;
	float maxValue = 1.f;
	float defaultValue = 1.f;
}paramEnvSustain;


const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Attack", "Env2Attack", "Env3Attack", "Env4Attack" };
	std::string name = "EnvAttack";
	std::string unitName = "ms";
	float minValue = log(0.1f);
	float maxValue = log(10000.f);
	float defaultValue = log(50.f);
}paramEnvAttack;

const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Decay", "Env2Decay", "Env3Decay", "Env4Decay" };
	std::string name = "EnvDecay";
	std::string unitName = "ms";
	float minValue = log(0.1f);
	float maxValue = log(10000.f);
	float defaultValue = log(50.f);
}paramEnvDecay;

const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Release", "Env2Release", "Env3Release", "Env4Release" };
	std::string name = "EnvRelease";
	std::string unitName = "ms";
	float minValue = log(0.1f);
	float maxValue = log(10000.f);
	float defaultValue = log(50.f);
}paramEnvRelease;

const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Level", "Env2Level", "Env3Level", "Env4Level" };
	std::string name = "EnvLevel";
	std::string unitName = "";
	float minValue = -90.f;
	float maxValue = 0.f;
	float defaultValue = 0.f;
}paramEnvLevel;

const struct
{
	const std::string ID[MAX_ENV_INSTANCES] = { "Env1Invert", "Env2Invert", "Env3Invert", "Env4Invert" };
	std::string name = "EnvInvert";
	std::string unitName = "";
	bool minValue = false;
	bool maxValue = true;
	bool defaultValue = false;
}paramEnvInvert;


class EnvelopeParameter
{
public:
	int addParameter(std::vector < std::unique_ptr<RangedAudioParameter>>& paramVector, int instance);
};

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class EnvelopeParameterComponent : public Component
{
public:
	enum class EnvelopeStyle
	{
		compact,
		horizontal,
		vertical
	};
	EnvelopeParameterComponent(AudioProcessorValueTreeState&, int index, const String& envName);

	void paint(Graphics& g) override;
	void resized() override;
	std::function<void()> somethingChanged;
	void setStyle(EnvelopeStyle style) { m_style = style; };
	void setShowDelay() { m_showdelay = true; };
	void setShowLevel() { m_showlevel = true; };
	void setShowInvert() { m_showInvert = true; };
private:
	AudioProcessorValueTreeState& m_vts;
	int m_index;
	String m_name;
	EnvelopeStyle m_style;
	bool m_showdelay;
	bool m_showlevel;
	bool m_showInvert;

	Label m_EnvAttackLabel;
	Slider m_EnvAttackSlider;
	std::unique_ptr<SliderAttachment> m_EnvAttackAttachment;

	Label m_EnvHoldLabel;
	Slider m_EnvHoldSlider;
	std::unique_ptr<SliderAttachment> m_EnvHoldAttachment;

	Label m_EnvDelayLabel;
	Slider m_EnvDelaySlider;
	std::unique_ptr<SliderAttachment> m_EnvDelayAttachment;

	Label m_EnvDecayLabel;
	Slider m_EnvDecaySlider;
	std::unique_ptr<SliderAttachment> m_EnvDecayAttachment;

	Label m_EnvSustainLabel;
	Slider m_EnvSustainSlider;
	std::unique_ptr<SliderAttachment> m_EnvSustainAttachment;

	Label m_EnvReleaseLabel;
	Slider m_EnvReleaseSlider;
	std::unique_ptr<SliderAttachment> m_EnvReleaseAttachment;

	Label m_EnvLevelLabel;
	Slider m_EnvLevelSlider;
	std::unique_ptr<SliderAttachment> m_EnvLevelAttachment;

	Label m_EnvInvertLabel;
	ToggleButton m_EnvInvertButton;
	std::unique_ptr<ButtonAttachment> m_EnvInvertAttachment;

	
};
#endif