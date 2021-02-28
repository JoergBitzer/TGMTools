/**
 * @file SmoothParameter.h
 * @author J. Bitzer @ Jade Hochschule (Hoertechnik und Audiologie)
 * @brief Two classes for smoothed parameter. The first smooth linearly the second logarithmic.
 * the smoothing time is set in ms ==> samplerate must be given
 * @version 1.0
 * @date 2021-02-28
 * 
 * @copyright Copyright (c) 2021 (MIT licence)
 * 
 */
#pragma once

#include <cmath>
/**
 * @brief a class to implement a linear smoothing for a parameter
 * 
 */
class SmoothParameterLinear
{
private:
    enum class SmoothStates
    {
        Off,
        Smoothing,
        NextValueSet
    };
public:
    /**
    * @brief Construct a new linear SmoothParameter object
    * default values are the samplingrate = 48000 and smoothing time = 2ms
    */
    SmoothParameterLinear()
    :m_fs(48000.0),m_smoothtime_ms(2.0),m_state(SmoothParameterLinear::SmoothStates::Off),m_curVal(0.f)
    {
        computeSmoothingTimes();
    }
    /**
     * @brief Construct a new linear SmoothParameter object
     * 
     * @param sampleRate : the sampling rate
     * @param smoothtime_ms : the desired smoothing time in ms
     */
    SmoothParameterLinear(float sampleRate, float smoothtime_ms)
    :m_fs(sampleRate),m_smoothtime_ms(smoothtime_ms),m_state(SmoothParameterLinear::SmoothStates::Off),m_curVal(0.f)
    {
        computeSmoothingTimes();
    }
    /**
     * @brief Set the Sample Rate 
     * 
     * @param sampleRate : the new sample rate
     */
    void setSampleRate(float sampleRate){m_fs = sampleRate;computeSmoothingTimes();};
    /**
     * @brief Set the smoothing Time in ms
     * 
     * @param smoothtime_ms 
     */
    void setSmoothTime(float smoothtime_ms){m_smoothtime_ms=smoothtime_ms;computeSmoothingTimes();};
    /**
     * @brief Get the Value first and Update later
     * 
     * @return float : the current value 
     */
    float getValueAndUpdate(){float Val = m_curVal; Update(); return Val;};
    /**
     * @brief Update first and get the new value of the smoothed parameter. This should be the default usage
     * 
     * @return float : the new smoothed value
     */
    float updateAndGetValue(){ Update(); return m_curVal;};
    /**
     * @brief Get the smoothed Value (no update)
     * 
     * @return float 
     */
    float getValue(){return m_curVal;};
    /**
     * @brief updates the internal smoothing. be careful and use this only if you want to skip values
     * 
     */
    void Update()
    {
        if (m_state != SmoothParameterLinear::SmoothStates::Off)
        {
            m_curVal += m_increment;
            
            if (--m_counter <= 0)
            {
                if (m_state == SmoothParameterLinear::SmoothStates::Smoothing)
                    m_state = SmoothParameterLinear::SmoothStates::Off;
                else
                {
                    m_state = SmoothParameterLinear::SmoothStates::Off;
                    setTargetValue(m_nextTargetVal);
                }
            }
        }
    };
    /**
     * @brief Set the startvalue for the parameter. This should be called only once after construction
     * 
     * @param startVal : the desired starting value (default  = 0.f)
     */
    void setStartValue(float startVal = 0.f){m_curVal = startVal;};
    /**
     * @brief Set the desired value after the smoothing process
     * 
     * @param targetVal : the desired new value
     */
    void setTargetValue(float targetVal)
    {
        switch (m_state)
        {
            case SmoothParameterLinear::SmoothStates::Off:
                m_increment = (targetVal-m_curVal)/m_smoothtimesamples;
                m_counter = m_smoothtimesamples;
                m_state = SmoothParameterLinear::SmoothStates::Smoothing;
            break;
            case SmoothParameterLinear::SmoothStates::Smoothing:
                m_nextTargetVal = targetVal;
                m_state = SmoothParameterLinear::SmoothStates::NextValueSet;
            break;
            case SmoothParameterLinear::SmoothStates::NextValueSet:
                m_nextTargetVal = targetVal;
            break;
        }
    };

private:
    float m_fs;
    float m_smoothtime_ms;
    float m_curVal;
    SmoothStates m_state;
    float m_increment;
    float m_nextTargetVal;
    int m_counter;
    int m_smoothtimesamples;
    void computeSmoothingTimes(){m_smoothtimesamples = m_smoothtime_ms*0.001*m_fs;}

};
/**
 * @brief a class to implement logarithmic smoothing for a parameter
 * 
 */

class SmoothParameterLog
{
private:    
    enum class SmoothStates
    {
        Off,
        Smoothing,
        NextValueSet
    };
public:
    /**
    * @brief Construct a new logarithmic SmoothParameter object
    * default values are the samplingrate = 48000 and smoothing time = 2ms
    */
    SmoothParameterLog()
    :m_fs(48000.f),m_smoothtime_ms(2.f),m_state(SmoothParameterLog::SmoothStates::Off),m_curVal(1.f)
    {
        computeSmoothingTimes();
    }
     /**
     * @brief Construct a new logarithmic SmoothParameter object
     * 
     * @param sampleRate : the sampling rate
     * @param smoothtime_ms : the desired smoothing time in ms
     */

    SmoothParameterLog(float sampleRate, float smoothtime_ms)
    :m_fs(sampleRate),m_smoothtime_ms(smoothtime_ms),m_state(SmoothParameterLog::SmoothStates::Off),m_curVal(1.f)
    {
        computeSmoothingTimes();
    }
    /**
     * @brief Set the Sample Rate 
     * 
     * @param sampleRate : the new sample rate
     */
    void setSampleRate(float sampleRate){m_fs = sampleRate;computeSmoothingTimes();};
    /**
     * @brief Set the smoothing Time in ms
     * 
     * @param smoothtime_ms 
     */
    void setSmoothTime(float smoothtime_ms){m_smoothtime_ms=smoothtime_ms;computeSmoothingTimes();};
     /**
     * @brief Get the Value first and Update later
     * 
     * @return float : the current value 
     */
    float getValueAndUpdate(){float Val = m_curVal; Update(); return Val;};
    /**
     * @brief Update first and get the new value of the smoothed parameter. This should be the default usage
     * 
     * @return float : the new smoothed value
     */
    float updateAndGetValue(){ Update(); return m_curVal;};
    /**
     * @brief Get the smoothed Value (no update)
     * 
     * @return float 
     */
    float getValue(){return m_curVal;};
    /**
     * @brief updates the internal smoothing. be careful and use this only if you want to skip values
     * 
     */
    void Update()
    {
        if (m_state != SmoothParameterLog::SmoothStates::Off)
        {
            m_curVal *= m_increment;
            
            if (--m_counter <= 0)
            {
                if (m_state == SmoothParameterLog::SmoothStates::Smoothing)
                    m_state = SmoothParameterLog::SmoothStates::Off;
                else
                {
                    m_state = SmoothParameterLog::SmoothStates::Off;
                    setTargetValue(m_nextTargetVal);
                }
            }
        }
    };
    /**
     * @brief Set the startvalue for the parameter. This should be called only once after construction
     * 
     * @param startVal : the desired starting value (default  = 0.f)
     */

    void setStartValue(float startVal){if (startVal>0.f) m_curVal = startVal;};
    /**
     * @brief Set the desired value after the smoothing process
     * 
     * @param targetVal : the desired new value
     */
    void setTargetValue(float targetVal)
    {
        if (targetVal<=0.f)
            return;
        switch (m_state)
        {
            case SmoothParameterLog::SmoothStates::Off:
                m_increment = (targetVal-m_curVal)/m_smoothtimesamples;
                m_increment = exp((log(targetVal) - log(m_curVal))/m_smoothtimesamples);
                m_counter = m_smoothtimesamples;
                m_state = SmoothParameterLog::SmoothStates::Smoothing;
            break;
            case SmoothParameterLog::SmoothStates::Smoothing:
                m_nextTargetVal = targetVal;
                m_state = SmoothParameterLog::SmoothStates::NextValueSet;
            break;
            case SmoothParameterLog::SmoothStates::NextValueSet:
                m_nextTargetVal = targetVal;
            break;
        }
    };

private:
    float m_fs;
    float m_smoothtime_ms;
    float m_curVal;
    SmoothStates m_state;
    float m_increment;
    float m_nextTargetVal;
    int m_counter;
    int m_smoothtimesamples;
    void computeSmoothingTimes(){m_smoothtimesamples = m_smoothtime_ms*0.001*m_fs;}
};

/* Usage example
    std::vector<double> data;
    data.resize(100);
    std::fill(data.begin(), data.end(), 0.0);
    SmoothParameterLog gain(48000.f,1.f);

    gain.setStartValue(1.f);
    gain.setTargetValue(0.1f);
    for (int bb = 0; bb < 99; bb++)
    {
        data[bb] = gain.getValueAndUpdate();
        if (bb == 30)
            gain.setTargetValue(2.0);
    }
//*/