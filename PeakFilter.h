#pragma once

#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "SOSFilter.h"

class PeakFilter
{
public:
//    PeakFilter();
//    PeakFilter(float fs);
//    PeakFilter(float fs, float f0);
//    PeakFilter(float fs, float f0, float Gain);
    PeakFilter(float fs = 44100.f, float f0 = 1000.f, float Gain = 0.f, float Q = 1.f)
    :m_fs(fs),m_f0(f0), m_gain(Gain),m_Q(Q){designCoeffs(); reset();};
    
    void setSamplerate(float fs){m_fs = fs; designCoeffs(); reset();};
    void setMidFrequency(float f0){m_f0 = f0; designCoeffs(); reset();};
    void setGain(float Gain){m_gain = Gain; designCoeffs(); reset();};
    void setQ(float Q){m_Q = Q; designCoeffs(); reset();};

    void reset(void){m_filter.reset();};
    int processSamples (float *in, float *out, int nrofsamples){
        return m_filter.processData(in, out, nrofsamples);};

private:
    int m_nrChns;
    float m_fs, m_f0, m_gain, m_Q;
    SOSFilter<float> m_filter;
    void designCoeffs(void)
    {
        float b[3], a[3];

        double omega = 2.*M_PI* (m_f0 / m_fs);

        double alpha = sin(omega / (2. * m_Q));
        double A = pow(10., m_gain / 40.);

        b[0] = 1. + alpha*A;
        b[1] = -2. * cos(omega);
        b[2] = 1. - alpha*A;
        a[0] = 1. + alpha / A;
        a[1] = b[1];
        a[2] = 1. - alpha / A;

        for (auto i = 2; i >= 0; i--) 
        {
            b[i] = b[i] / a[0];
            a[i] = a[i] / a[0];
        }
        m_filter.setCoeffs(b[0],b[1],b[2],a[1],a[2]);
    };
};