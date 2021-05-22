#pragma once
/**
 * @file SOSFilter.h
 * @author J. Bitzer @ TGM @ JAde Hochschule
 * @brief This class is a second order section filter, It provides clickfree time variant processing
 * the smoothing is done via blending not via morphing (fast, but sonically sub-optimal)
 * @version 0.1
 * @date 2021-05-22
 * 
 * @copyright Copyright (c) 2021 Licence: BSD 3-clause
 * 
 */
/*
    1) change to template class for double precision
//*/
#include <vector>
class SOSFilter
{
public:
    SOSFilter(){m_b0 = 1.0; m_b1 = 0.0; m_b2 = 0.0; m_a1 = 0.0; m_a2 = 0.0;
        m_b0Old = 1.0; m_b1Old = 0.0; m_b2Old = 0.0; m_a1Old = 0.0; m_a2Old = 0.0;
   	    m_newCoeffs = false; setXFadeSamples(30);
        reset(); };
    SOSFilter(float b0, float b1, float b2, float a1, float a2){m_b0 = b0; m_b1 = b1; m_b2 = b2; m_a1 = a1; m_a2 = a2;
        m_b0Old = b0; m_b1Old = b1; m_b2Old = b2; m_a1Old = a1; m_a2Old = a2;
   	    m_newCoeffs = false; setXFadeSamples(30);
        reset(); };

    void reset(){
        m_statea1 = 0.0; m_statea2 = 0.0; m_stateb1 = 0.0; m_stateb2 = 0.0;
        m_statea1Old = 0.0; m_statea2Old = 0.0;};

	int setCoeffs(float b0, float b1, float b2, float a1, float a2){
        m_b0Old = m_b0; m_b1Old = m_b1; m_b2Old = m_b2; m_a1Old = m_a1; m_a2Old = m_a2;
        m_statea1Old = m_statea1; m_statea2Old = m_statea2;
        m_b0 = b0; m_b1 = b1; m_b2 = b2; m_a1 = a1; m_a2 = a2;
        m_newCoeffs = true; m_xFadeCounter = 0; m_CrossGain = 0.0;};
	
	int processData(std::vector<double>& in, std::vector<double>& out)
    {
	    if (in.size() != out.size())
		    return -1;

	    for (unsigned int kk = 0; kk < in.size(); kk++)
	    {
            float curSample = in[kk];
			out[kk] = m_b0*curSample + m_b1*m_stateb1 + m_b2*m_stateb2 - m_a1*m_statea1 - m_a2*m_statea2;
    		m_statea2 = m_statea1;
            m_statea1 = out[kk];
            m_stateb2 = m_stateb1;
            m_stateb1 = curSample;
    	}

	    return 0;
    };
	int processDataTV(std::vector<double>& in, std::vector<double>& out)
    {
	    if (in.size() != out.size())
		    return -1;

	    if (m_newCoeffs == false)
	    {
		    processData(in, out);
	    }
	    else // TV cross fade audio
	    {
		    for (unsigned int kk = 0; kk < in.size(); kk++)
		    {
			    float newOut = 0.0;
			    float oldOut = 0.0;
                float curSample = in[kk];
			    newOut = m_b0*curSample + m_b1*m_stateb1 + m_b2*m_stateb2 ;
                oldOut = m_b0Old*curSample + m_b1Old*m_stateb1 + m_b2Old*m_stateb2 ;
    			newOut -= (m_a1*m_statea1 + m_a2*m_statea2);
    			oldOut -= (m_a1Old*m_statea1Old + m_a2Old*m_statea2Old);
    		
                m_statea2 = m_statea1;
                m_statea1 = newOut;
                m_statea2Old = m_statea1Old;
                m_statea1Old = oldOut;
                m_stateb2 = m_stateb1;
                m_stateb1 = curSample;
			    if (m_xFadeCounter < m_xFadeTimeSamples)
			    {
				    out[kk] = (1.0 - m_CrossGain) * oldOut + m_CrossGain * newOut;
				    m_CrossGain += m_StepSize;
				    m_xFadeCounter++;
			    }
			    else
			    {
				    out[kk] = newOut;
				    m_newCoeffs = false;
			    }
		}
		
	}

	return 0;        
    }
	int setXFadeSamples(int nrofsamples)
    {
        m_xFadeTimeSamples = nrofsamples;
	    m_xFadeCounter = 0;
	    m_StepSize = 1.0 / (m_xFadeTimeSamples - 1);
	    m_CrossGain = 0.0;
	    return 0;
    }


private:
    float m_b0,m_b1,m_b2;
    float m_a1,m_a2;

    float m_stateb1,m_stateb2;
    float m_statea1,m_statea2;

	bool m_newCoeffs;
	int m_xFadeCounter;
	int m_xFadeTimeSamples;
	float m_CrossGain;
	float m_StepSize;

    float m_b0Old,m_b1Old,m_b2Old;
    float m_a1Old,m_a2Old;
    float m_statea1Old,m_statea2Old;

};
