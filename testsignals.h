
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <random>

inline void genSinusSweep(float* buffer, int numSamples, float freqStart, float freqEnd, float samplerate)
{
    float phase = 0;
    float phaseIncr = 2 * M_PI * freqStart / samplerate;
    float phaseIncrStep = 2 * M_PI * (freqEnd - freqStart) / numSamples / samplerate;
    for (int i = 0; i < numSamples; i++)
    {
        buffer[i] = sinf(phase);
        phase += phaseIncr;
        phaseIncr += phaseIncrStep;
    }
}
inline void deltaimpulse(float* buffer, int numSamples, int delay = 0)
{
    for (int i = 0; i < numSamples; i++)
    {
        buffer[i] = 0.f;
    }
    if (delay < numSamples)
        buffer[delay] = 1.f;
    else
        buffer[0] = 1.f;
}

// sinus tone
inline void sinusTone(float* buffer, int numSamples, float freq, float samplerate)
{
    float phase = 0;
    float phaseIncr = 2 * M_PI * freq / samplerate;
    for (int i = 0; i < numSamples; i++)
    {
        buffer[i] = sinf(phase);
        phase += phaseIncr;
    }
}

// noise white urn
inline void noiseWhiteURN(float* buffer, int numSamples, float minValue = -1.f, float maxValue = 1.f)
{
    std::random_device rd{};
    std::mt19937 gen{rd()};
 
    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::uniform_real_distribution d{minValue, maxValue};

    for (int i = 0; i < numSamples; i++)
    {
        buffer[i] = d(gen);
    }
}

// noise gaussian
inline void noiseWhiteGaussian(float* buffer, int numSamples, float meanValue, float standardDeviation)
{
    std::random_device rd{};
    std::mt19937 gen{rd()};
 
    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution d{meanValue, standardDeviation};

    for (int i = 0; i < numSamples; i++)
    {
        buffer[i] = d(gen);
    }
}




