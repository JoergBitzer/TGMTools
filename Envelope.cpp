#include "Envelope.h"


Envelope::Envelope()
{
	m_envName = "Env";
	m_fs = 1.0;

	// initial values
	m_delTime_ms = paramEnvDelay.defaultValue;
	m_tauAtt_ms	= exp(paramEnvAttack.defaultValue);
	m_tauDec_ms	= exp(paramEnvDecay.defaultValue);
	m_attOver = 1.0 / 0.77; // from Will Pirkle Book
	m_holdTime_ms = paramEnvHold.defaultValue;
	m_tauRel_ms	= exp(paramEnvRelease.defaultValue);
	m_sustainLevel = paramEnvSustain.defaultValue;
	m_maxLevel = paramEnvLevel.defaultValue;
	m_invertOn = paramEnvInvert.defaultValue;
	m_envelopePhase = envelopePhases::Off;
	m_envGain = 0.f;
	m_envOut = 0.f;
	updateTimeConstants();
	reset();
}

Envelope::~Envelope()
{
}

int Envelope::getData(std::vector<double>& data)
{
	for (unsigned int kk = 0; kk < data.size(); kk++)
	{
		data[kk] = updateEnvelopePhase();
	}
	return 0;
}

void Envelope::NoteOn()
{
	m_envelopePhase = envelopePhases::Delay;
	m_sampleCounter = m_delSamples;
}

void Envelope::NoteOff()
{
	m_envelopePhase = envelopePhases::Release;
	if (m_envGain < 1.0e-5)
		m_envelopePhase = envelopePhases::Off;
}

void Envelope::setSamplerate(double samplerate)
{
	m_fs = samplerate;
	updateTimeConstants();
}

void Envelope::setDelayTime(double del_ms)
{
	m_delTime_ms = del_ms;
	updateTimeConstants();
}

void Envelope::setAttackRate(double att_ms)
{
	m_tauAtt_ms = att_ms;
	updateTimeConstants();
}

void Envelope::setHoldTime(double hold_ms)
{
	m_holdTime_ms = hold_ms;
	updateTimeConstants();
}

void Envelope::setDecayRate(double dec_ms)
{
	m_tauDec_ms = dec_ms;
	updateTimeConstants();
}

void Envelope::setSustainLevel(double level)
{
	m_sustainLevel = level;
}

void Envelope::setReleaseRate(double rel_ms)
{
	m_tauRel_ms = rel_ms;
	updateTimeConstants();
}

void Envelope::updateTimeConstants(void)
{
	m_alphaAtt = exp(-1.0 / (m_tauAtt_ms * 0.001 * m_fs));
	m_alphaDec = exp(-1.0 / (m_tauDec_ms * 0.001 * m_fs));
	m_alphaRel = exp(-1.0 / (m_tauRel_ms * 0.001 * m_fs));
	m_holdSamples = floor(m_holdTime_ms * m_fs * 0.001 + 0.5);
	m_delSamples = floor(m_delTime_ms * m_fs * 0.001 + 0.5);

}

int Envelope::updateParameter()
{
    float newval = *(m_envparams.m_delay);
    if ( newval != m_envparams.m_delayOld)
    {
        m_envparams.m_delayOld = newval;
        setDelayTime(newval);
    }
    newval = *(m_envparams.m_attack);
    if ( newval != m_envparams.m_attackOld)
    {
        m_envparams.m_attackOld = newval;
        setAttackRate(exp(newval));
    }
    newval = *(m_envparams.m_hold);
    if ( newval != m_envparams.m_holdOld)
    {
        m_envparams.m_holdOld = newval;
        setHoldTime(newval);
    }
    newval = *(m_envparams.m_decay);
    if ( newval != m_envparams.m_decayOld)
    {
        m_envparams.m_decayOld = newval;
        setDecayRate(exp(newval));
    }
    newval = *(m_envparams.m_sustain);
    if ( newval != m_envparams.m_sustainOld)
    {
        m_envparams.m_sustainOld = newval;
        setSustainLevel(newval);
    }
    newval = *(m_envparams.m_release);
    if ( newval != m_envparams.m_releaseOld)
    {
        m_envparams.m_releaseOld = newval;
        setReleaseRate(exp(newval));
    }
    newval = *(m_envparams.m_inverted);
    if ( newval != m_envparams.m_invertedOld)
    {
        m_envparams.m_invertedOld = newval;
        setInvertOnOff(newval);
    }
    newval = *(m_envparams.m_level);
    if ( newval != m_envparams.m_levelOld)
    {
        m_envparams.m_levelOld = newval;
        setMaxLevel(newval);
    }
	return 0;

}

#ifdef USE_JUCE
int EnvelopeParameter::addParameter(std::vector < std::unique_ptr<RangedAudioParameter>>& paramVector, int instance)
{
	if (instance < MAX_ENV_INSTANCES)
	{
		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvAttack.ID[instance],
			paramEnvAttack.name,
			NormalisableRange<float>(paramEnvAttack.minValue, paramEnvAttack.maxValue),
			paramEnvAttack.defaultValue,
			paramEnvAttack.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(int(exp(value) * 10 + 0.5) * 0.1, MaxLen) + " ms"); },
			[](const String& text) {return text.getFloatValue(); }));

		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvDecay.ID[instance],
			paramEnvDecay.name,
			NormalisableRange<float>(paramEnvDecay.minValue, paramEnvDecay.maxValue),
			paramEnvDecay.defaultValue,
			paramEnvDecay.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(int(exp(value) * 10 + 0.5) * 0.1, MaxLen) + " ms"); },
			[](const String& text) {return text.getFloatValue(); }));
		
		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvRelease.ID[instance],
			paramEnvRelease.name,
			NormalisableRange<float>(paramEnvRelease.minValue, paramEnvRelease.maxValue),
			paramEnvRelease.defaultValue,
			paramEnvRelease.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(int(exp(value) * 10 + 0.5) * 0.1, MaxLen) + " ms"); },
			[](const String& text) {return text.getFloatValue(); }));

		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvDelay.ID[instance],
			paramEnvDelay.name,
			NormalisableRange<float>(paramEnvDelay.minValue, paramEnvDelay.maxValue),
			paramEnvDelay.defaultValue,
			paramEnvDelay.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(0.1*int((value) * 10 + 0.5), MaxLen) + " ms"); },
			[](const String& text) {return text.getFloatValue(); }));

		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvHold.ID[instance],
			paramEnvHold.name,
			NormalisableRange<float>(paramEnvHold.minValue, paramEnvHold.maxValue),
			paramEnvHold.defaultValue,
			paramEnvHold.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(0.1 * int((value) * 10 + 0.5), MaxLen) + " ms"); },
			[](const String& text) {return text.getFloatValue(); }));

		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvSustain.ID[instance],
			paramEnvSustain.name,
			NormalisableRange<float>(paramEnvSustain.minValue, paramEnvSustain.maxValue),
			paramEnvSustain.defaultValue,
			paramEnvSustain.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(0.01 * int((value) * 100.0 + 0.5), MaxLen) + ""); },
			[](const String& text) {return text.getFloatValue(); }));

		paramVector.push_back(std::make_unique<AudioParameterFloat>(paramEnvLevel.ID[instance],
			paramEnvLevel.name,
			NormalisableRange<float>(paramEnvLevel.minValue, paramEnvLevel.maxValue),
			paramEnvLevel.defaultValue,
			paramEnvLevel.unitName,
			AudioProcessorParameter::genericParameter,
			[](float value, int MaxLen) { return (String(0.1 * int((value) * 10 + 0.5), MaxLen) + ""); },
			[](const String& text) {return text.getFloatValue(); }));

		paramVector.push_back(std::make_unique<AudioParameterBool>(paramEnvInvert.ID[instance],
			paramEnvInvert.name,
			paramEnvInvert.defaultValue,
			paramEnvInvert.unitName));
	}
	return 0;
}

int EnvelopeParameter::connectParameter(AudioProcessorValueTreeState* vts, int instance)
{
    m_delay = vts->getRawParameterValue(paramEnvDelay.ID[instance]);
    m_delayOld = *m_delay;
    m_attack = vts->getRawParameterValue(paramEnvAttack.ID[instance]);
    m_attackOld = *m_attack;     

    m_hold = vts->getRawParameterValue(paramEnvHold.ID[instance]);
    m_holdOld = *m_hold;
    m_decay = vts->getRawParameterValue(paramEnvDecay.ID[instance]);
    m_decayOld = *m_decay;     

    m_sustain = vts->getRawParameterValue(paramEnvSustain.ID[instance]);
    m_sustainOld = *m_sustain;
    m_release = vts->getRawParameterValue(paramEnvRelease.ID[instance]);
    m_releaseOld = *m_release;     

    m_level = vts->getRawParameterValue(paramEnvLevel.ID[instance]);
    m_levelOld = *m_level;
    m_inverted = vts->getRawParameterValue(paramEnvInvert.ID[instance]);
    m_invertedOld = *m_inverted;     
	return 0;
}



EnvelopeParameterComponent::EnvelopeParameterComponent(AudioProcessorValueTreeState& vts, int index, const String& envName)
	:somethingChanged(nullptr), m_vts(vts),   m_index(index),m_name(envName),
	m_style(EnvelopeStyle::horizontal), m_showdelay(false),m_showhold(false),m_showlevel(false),m_ScaleFactor(1.f), m_showInvert(false)
{

	m_EnvDelayLabel.setText("Delay", NotificationType::dontSendNotification);
	m_EnvDelayLabel.setJustificationType(Justification::centred);
	m_EnvDelayLabel.attachToComponent (&m_EnvDelaySlider, false);
	addAndMakeVisible(m_EnvDelayLabel);

	m_EnvDelaySlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvDelaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvDelayAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvDelay.ID[m_index], m_EnvDelaySlider);
	addAndMakeVisible(m_EnvDelaySlider);
	m_EnvDelaySlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };

	m_EnvAttackLabel.setText("Attack", NotificationType::dontSendNotification);
	m_EnvAttackLabel.setJustificationType(Justification::centred);
	m_EnvAttackLabel.attachToComponent (&m_EnvAttackSlider, false);
	addAndMakeVisible(m_EnvAttackLabel);

	m_EnvAttackSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvAttackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvAttackAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvAttack.ID[m_index], m_EnvAttackSlider);
	addAndMakeVisible(m_EnvAttackSlider);
	m_EnvAttackSlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };
	//m_EnvAttackSlider.setValue(paramEnvAttack.defaultValue,NotificationType::sendNotification);

	m_EnvHoldLabel.setText("Hold", NotificationType::dontSendNotification);
	m_EnvHoldLabel.setJustificationType(Justification::centred);
	m_EnvHoldLabel.attachToComponent (&m_EnvHoldSlider, false);

	addAndMakeVisible(m_EnvHoldLabel);

	m_EnvHoldSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvHoldSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvHoldAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvHold.ID[m_index], m_EnvHoldSlider);
	addAndMakeVisible(m_EnvHoldSlider);
	m_EnvHoldSlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };

	m_EnvDecayLabel.setText("Decay", NotificationType::dontSendNotification);
	m_EnvDecayLabel.setJustificationType(Justification::centred);
	m_EnvDecayLabel.attachToComponent (&m_EnvDecaySlider, false);

	addAndMakeVisible(m_EnvDecayLabel);

	m_EnvDecaySlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvDecaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvDecayAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvDecay.ID[m_index], m_EnvDecaySlider);
	addAndMakeVisible(m_EnvDecaySlider);
	m_EnvDecaySlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };

	m_EnvSustainLabel.setText("Sustain", NotificationType::dontSendNotification);
	m_EnvSustainLabel.setJustificationType(Justification::centred);
	m_EnvSustainLabel.attachToComponent (&m_EnvSustainSlider, false);

	addAndMakeVisible(m_EnvSustainLabel);

	m_EnvSustainSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvSustainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvSustainAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvSustain.ID[m_index], m_EnvSustainSlider);
	addAndMakeVisible(m_EnvSustainSlider);
	m_EnvSustainSlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };

	m_EnvReleaseLabel.setText("Release", NotificationType::dontSendNotification);
	m_EnvReleaseLabel.setJustificationType(Justification::centred);
	m_EnvReleaseLabel.attachToComponent (&m_EnvReleaseSlider, false);

	addAndMakeVisible(m_EnvReleaseLabel);

	m_EnvReleaseSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvReleaseSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvReleaseAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvRelease.ID[m_index], m_EnvReleaseSlider);
	addAndMakeVisible(m_EnvReleaseSlider);
	m_EnvReleaseSlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };

	m_EnvLevelLabel.setText("Level", NotificationType::dontSendNotification);
	m_EnvLevelLabel.setJustificationType(Justification::centred);
	m_EnvLevelLabel.attachToComponent (&m_EnvLevelSlider, false);

	addAndMakeVisible(m_EnvLevelLabel);

	m_EnvLevelSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	m_EnvLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, ENV_LABEL_WIDTH, ENV_LABEL_HEIGHT);
	m_EnvLevelAttachment = std::make_unique<SliderAttachment>(m_vts, paramEnvLevel.ID[m_index], m_EnvLevelSlider);
	addAndMakeVisible(m_EnvLevelSlider);
	m_EnvLevelSlider.onValueChange = [this]() {if (somethingChanged != nullptr) somethingChanged(); };

	m_EnvInvertLabel.setText("Invert", NotificationType::dontSendNotification);
	m_EnvInvertLabel.setJustificationType(Justification::centred);
	m_EnvInvertLabel.attachToComponent (&m_EnvInvertButton, false);
	addAndMakeVisible(m_EnvInvertLabel);

	m_EnvInvertButton.setButtonText("On");
	
	m_EnvInvertButton.setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::darkgrey);
	m_EnvInvertButton.setColour(ToggleButton::ColourIds::textColourId, Colours::darkgrey);
	m_EnvInvertButton.setColour(ToggleButton::ColourIds::tickColourId, Colours::darkgrey);

	m_EnvInvertAttachment = std::make_unique<ButtonAttachment>(m_vts, paramEnvInvert.ID[m_index],m_EnvInvertButton );
	addAndMakeVisible(m_EnvInvertButton);

}
void EnvelopeParameterComponent::paint(Graphics& g)
{
	g.fillAll((getLookAndFeel().findColour(ResizableWindow::backgroundColourId)).darker(0.2));
}


void EnvelopeParameterComponent::resized()
{

	float scaleFactor = m_ScaleFactor;
	auto r = getLocalBounds();
	r.reduce(scaleFactor*ENV_MIN_DISTANCE,scaleFactor*ENV_MIN_DISTANCE);
	auto s = r;
	auto t = r;
	switch (m_style)
	{
	case EnvelopeStyle::compact:

		break;
	case EnvelopeStyle::horizontal:
		t = s.removeFromBottom(scaleFactor*ENV_ROTARY_WIDTH);
		if (m_showdelay)
		{
			m_EnvDelaySlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
			t.removeFromLeft(scaleFactor*ENV_MIN_DISTANCE);
			m_EnvDelaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);

		}
		m_EnvAttackSlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
		m_EnvAttackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);
		t.removeFromLeft(scaleFactor*ENV_MIN_DISTANCE);
		if (m_showhold)
		{	
			m_EnvHoldSlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
			m_EnvHoldSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);
			t.removeFromLeft(scaleFactor*ENV_MIN_DISTANCE);
		}
		m_EnvDecaySlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
		m_EnvDecaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);

		t.removeFromLeft(scaleFactor*ENV_MIN_DISTANCE);
		m_EnvSustainSlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
		m_EnvSustainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);
		
		t.removeFromLeft(scaleFactor*ENV_MIN_DISTANCE);
		m_EnvReleaseSlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
		m_EnvReleaseSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);

		if (m_showlevel)
		{
			t.removeFromLeft(scaleFactor*ENV_MIN_DISTANCE);
			m_EnvLevelSlider.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH));
			m_EnvLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true,scaleFactor* ENV_LABEL_WIDTH, scaleFactor*ENV_LABEL_HEIGHT);

		}
		if (m_showInvert)
		{
			t.removeFromLeft(scaleFactor*2*ENV_MIN_DISTANCE);
			m_EnvInvertButton.setBounds(t.removeFromLeft(scaleFactor*ENV_ROTARY_WIDTH).removeFromTop(40));
		}

		break;
	case EnvelopeStyle::vertical:

		break;
	}
}
#endif
