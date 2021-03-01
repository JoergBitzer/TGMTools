#include "BrickwallLimiter.h"

#include <cmath>

BrickwallLimiter::BrickwallLimiter()
:m_fs(48000.0),m_nrofchannels(2),m_Limit(1.f),m_attackTime_ms(2.f),m_releaseTime_ms(100.f),m_Gain(1.f),
m_attackCounter(0),m_state(BrickwallLimiter::State::Off)
{
    buildAndResetDelayLine();
}
BrickwallLimiter::BrickwallLimiter(float sampleRate)
:m_fs(sampleRate),m_nrofchannels(2), m_Limit(1.f),m_attackTime_ms(2.f),m_releaseTime_ms(100.f),m_Gain(1.f),
m_attackCounter(0),m_state(BrickwallLimiter::State::Off)
{
    buildAndResetDelayLine();
}

void BrickwallLimiter::prepareToPlay(float sampleRate, int nrofchannels)
{
    m_fs = sampleRate;
    m_nrofchannels = nrofchannels;
    buildAndResetDelayLine();
}
int BrickwallLimiter::processSamples(std::vector<std::vector<float>>& data)
{
    size_t nrOfInputChannels= data.size();
    size_t nrOfSamples = data[0].size();

    for (auto kk = 0; kk < nrOfSamples; ++kk)
    {
        float maxVal = -1000.f;
        // put the input into delay and look for maximum over all channels
        for (auto cc = 0; cc < nrOfInputChannels; ++cc)
        {
            float inVal = data[cc][kk];
            m_delayline.at(cc).push(inVal);
            if (fabs(inVal)>maxVal)
                maxVal = fabs(inVal);
        }
        //float maxValwithGain = maxVal*(m_Gain+m_attackCounter*m_attackIncrement);
        float maxValwithGain = maxVal*(m_Gain);

        if (maxValwithGain>m_Limit)
        {
            m_state = BrickwallLimiter::State::Att;
            
            float attackIncrement = -(m_Gain-m_Gain/(maxValwithGain))/m_delaySamples;
            if (attackIncrement < m_attackIncrement)
            {
                m_attackCounter = m_delaySamples;
                m_attackIncrement = attackIncrement;
            }
            else
            {
                int nrofstepsneeded = int(-(m_Gain-m_Gain/(maxValwithGain))/m_attackIncrement)+1;
                m_attackCounter = nrofstepsneeded;
            }
        }
        switch (m_state)
        {
        case BrickwallLimiter::State::Off:
            
            break;
        case BrickwallLimiter::State::Att:
            m_Gain += m_attackIncrement;
            m_attackCounter--;
            if (m_attackCounter <= 0)
            {
                m_state = BrickwallLimiter::State::Hold;
                m_holdCounter = m_delaySamples;
                m_attackIncrement = 0.f;
            }
            break;
        case BrickwallLimiter::State::Hold:
            m_holdCounter--;
            if (m_holdCounter <= 0)            
            {
                m_state = BrickwallLimiter::State::Rel;
            }
            break;
        case BrickwallLimiter::State::Rel:
            m_Gain = m_Gain*m_alphaRelease + (1.0-m_alphaRelease)*1.01;
            if (m_Gain>=1.f)
            {
                m_state = BrickwallLimiter::State::Off;
                m_Gain = 1.f;
            }
            break;
        
        default:
            break;
        }


        // Get the data out of delay line and multiply with gain
        for (auto cc = 0; cc < nrOfInputChannels; ++cc)
        {
            float outVal = m_delayline.at(cc).front();
            m_delayline.at(cc).pop();
            data[cc][kk] = outVal * m_Gain;

            // DBG
            if (cc == 1) data[cc][kk] = m_Gain;

            if (data[cc][kk]>1.f)
                data[cc][kk] = outVal * m_Gain;
        }


    }
    return 0;
}
void BrickwallLimiter::buildAndResetDelayLine()
{
    m_delaySamples = int(m_attackTime_ms*0.001*m_fs + 0.5);
    m_delayline.resize(m_nrofchannels);

    for (size_t cc = 0; cc < m_delayline.size(); ++cc)
    {
        m_delayline.at(cc) = {};
        for (size_t kk = 0 ; kk < m_delaySamples-1; kk++)
            m_delayline.at(cc).push(0.f);
    }

    m_alphaRelease = exp(-1.f/(m_attackTime_ms*0.001f*m_fs));
}
