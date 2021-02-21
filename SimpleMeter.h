/**
 * @file SimpleMeter.h
 * @author J. Bitzer @ Jade Hochschule
 * @brief This component implments a Levelmeter with peak and rms display in JUCE
 * It contains of the audio analysis class and the display class. 
 * The geometry of the component defines if the displayed meter is vertical or horizontal
 * @version 1.0
 * @date 2021-02-21
 * 
 * @copyright Copyright (c) 2021
 * 
 * TODO:
 * Add scale on rightside or below
 * add more comments
 * 
 */
#pragma once

#include <vector>

#include "JuceHeader.h"


class SimpleMeter
{
public:
    SimpleMeter();
    void prepareToPlay (float samplerate, int SamplesPerBlock);
    void analyseData (juce::AudioBuffer<float>& data);

    int getAnalyserData(std::vector<float>& rms, std::vector<float>& peak);

private:
    float m_fs;
    int m_blockSize;
    const size_t m_maxnrofchannels = 8;

    double m_tauAttRMS_ms;
    double m_alphaAttRMS;
    double m_tauRelRMS_ms;
    double m_alphaRelRMS;

    int m_holdtime_ms;
    int m_holdtime_samples;

    std::vector<float> m_rms;
    std::vector<float> m_peak;
    std::vector<float> m_peakholdcounter;

    void computeTimeConstants();
    void reset();
};

class SimpleMeterComponent : public Component, public Timer
{
public:
    SimpleMeterComponent(SimpleMeter &meter);
    ~SimpleMeterComponent(){stopTimer();};
	void paint(Graphics& g) override;
	void resized() override;
    void setScaleFactor(float newscale){m_scaleFactor = newscale;};
    void timerCallback() override
    {
        m_displaychannels = m_meter.getAnalyserData(m_rms,m_peak);
        repaint();
    };
private:
    float m_scaleFactor;
    SimpleMeter& m_meter;
    std::vector<float> m_rms;
    std::vector<float> m_peak;
    size_t m_displaychannels; 

};