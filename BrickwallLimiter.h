#pragma once

#include <vector>
#include <queue>
#include <JuceHeader.h>

class BrickwallLimiterParameter
{
public:
	int addParameter(std::vector < std::unique_ptr<RangedAudioParameter>>& paramVector);

public:
    std::atomic<float>* m_onoff;
    float m_onoffOld;
};


class BrickwallLimiter
{
public:
    BrickwallLimiter();
    BrickwallLimiter(double sampleRate);
    void prepareParameter(std::unique_ptr<AudioProcessorValueTreeState>& vts);
    void prepareToPlay(double sampleRate, int nrofchannels);
    int processSamples(std::vector<std::vector<double>>& data);

    void setGainLimit(double newLimit){m_Limit = newLimit;};
    void setReleaseTime(double releaseTime);
    double getReduction(){return m_curReduction;};
    int getDelaySamples(){return m_delaySamples;};
    double getReduction_db(){return 20.0*log10(m_Gain+0.00000001);};
private:
    enum class State
    {
        Off,
        Att,
        Hold,
        Rel
    };

    double m_fs;
    int m_nrofchannels;
    double m_Limit;
    double m_curReduction;
    int m_delaySamples;
    double m_releaseTime_ms;
    double m_alphaRelease;
    std::vector<std::queue <double>> m_delayline;
    double m_attackTime_ms;
    double m_Gain;
    int m_attackCounter;
    int m_holdCounter;

    double m_attackIncrement;
    BrickwallLimiter::State m_state;
    void buildAndResetDelayLine();
    BrickwallLimiterParameter m_brickwallLimiterparamter;
};

const struct
{
	const std::string ID = "BLOnOff";
	std::string name = "OnOff";
	std::string unitName = "";
	float minValue = 0.f;
	float maxValue = 1.f;
	float defaultValue = 1.f;
}paramBrickwallLimiterOnOff;

class BrickwallLimiterComponent : public Component, public Timer
{
public:
	BrickwallLimiterComponent(AudioProcessorValueTreeState& , BrickwallLimiter& );

	void paint(Graphics& g) override;
	void resized() override;
    std::function<void()> somethingChanged;
    void setScaleFactor(float newscale){m_scaleFactor = newscale;};
    void timerCallback() override
    {
        m_reduction = m_limiter.getReduction_db();
        repaint();
    };
private:
    AudioProcessorValueTreeState& m_vts; 
    BrickwallLimiter& m_limiter;
    ToggleButton m_onoff;
    float m_reduction;
    float m_scaleFactor;

};

