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

/* from musicdsp.org. This should make things clearer in the code
float MMtoDB(float mm)
{
        float db;

        mm = 100. - mm;

        if (mm <= 0.) {
                db = 10.;
        } else if (mm < 48.) {
                db = 10. - 5./12. * mm;
        } else if (mm < 84.) {
                db = -10. - 10./12. * (mm - 48.);
        } else if (mm < 96.) {
                db = -40. - 20./12. * (mm - 84.);
        } else if (mm < 100.) {
                db = -60. - 35. * (mm - 96.);
        } else db = -200.;
        return db;
}

float DBtoMM(float db)
{
        float mm;
        if (db >= 10.) {
                mm = 0.;
        } else if (db > -10.) {
                mm = -12./5. * (db - 10.);
        } else if (db > -40.) {
                mm = 48. - 12./10. * (db + 10.);
        } else if (db > -60.) {
                mm = 84. - 12./20. * (db + 40.);
        } else if (db > -200.) {
                mm = 96. - 1./35. * (db + 60.);
        } else mm = 100.;

        mm = 100. - mm;

        return mm;
}

//*/