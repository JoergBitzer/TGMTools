#pragma once

#include <vector>
#include <queue>

class BrickwallLimiter
{
public:
    BrickwallLimiter();
    BrickwallLimiter(float sampleRate);

    void prepareToPlay(float sampleRate, int nrofchannels);
    int processSamples(std::vector<std::vector<float>>& data);

    void setGainLimit(float newLimit){m_Limit = newLimit;};
    void setReleaseTime(float releaseTime);
    float getReduction(){return m_curReduction;};
    int getDelaySamples(){return m_delaySamples;};

private:
    enum class State
    {
        Off,
        Att,
        Hold,
        Rel
    };

    float m_fs;
    int m_nrofchannels;
    float m_Limit;
    float m_curReduction;
    int m_delaySamples;
    float m_releaseTime_ms;
    float m_alphaRelease;
    std::vector<std::queue <float>> m_delayline;
    float m_attackTime_ms;
    float m_Gain;
    int m_attackCounter;
    int m_holdCounter;

    float m_attackIncrement;
    BrickwallLimiter::State m_state;

    void buildAndResetDelayLine();
};
