#pragma once

#include <vector>
#include <queue>

class BrickwallLimiter
{
public:
    BrickwallLimiter();
    BrickwallLimiter(double sampleRate);

    void prepareToPlay(double sampleRate, int nrofchannels);
    int processSamples(std::vector<std::vector<double>>& data);

    void setGainLimit(double newLimit){m_Limit = newLimit;};
    void setReleaseTime(double releaseTime);
    double getReduction(){return m_curReduction;};
    int getDelaySamples(){return m_delaySamples;};

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
};
