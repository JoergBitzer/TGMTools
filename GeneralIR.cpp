#include "GeneralIR.h"

GeneralIR::GeneralIR()
{
	m_b.resize(1);
	m_a.resize(1);
	m_b[0] = 1.0;
	m_a[0] = 1.0;
	m_bStates.resize(m_b.size());
	m_aStates.resize(m_a.size());
	m_bOld = m_b;
	m_aOld = m_a;
	m_aStatesOld.resize(m_aOld.size());

	m_newCoeffs = false;
}

GeneralIR::GeneralIR(std::vector<double> b, std::vector<double> a)
{
	m_b = b;
	m_a = a;
	m_bOld = b;
	m_aOld = a;
	m_aStatesOld.resize(m_aOld.size());
	m_bStates.resize(m_b.size());
	m_aStates.resize(m_a.size());
	std::fill(m_bStates.begin(), m_bStates.end(), 0.0);
	std::fill(m_aStates.begin(), m_aStates.end(), 0.0);
	m_newCoeffs = false;
}

int GeneralIR::setCoeffs(std::vector<double> b, std::vector<double> a)
{
	m_bOld = m_b;
	m_aOld = m_a;
	m_aStatesOld = m_aStates;

	if (m_b.size() == b.size())
		m_b = b;
	else
	{
		m_b = b;
		m_bStates.resize(m_b.size());
		std::fill(m_bStates.begin(), m_bStates.end(), 0.0);
	}
	if (m_a.size() == a.size())
		m_a = a;
	else
	{
		m_a = a;
		m_aStates.resize(m_a.size());
		std::fill(m_aStates.begin(), m_aStates.end(), 0.0);
	}
	m_newCoeffs = true;
	return 0;
}

int GeneralIR::processData(std::vector<double>& in, std::vector<double>& out)
{
	if (in.size() != out.size())
		return -1;

	for (unsigned int kk = 0; kk < in.size(); kk++)
	{
		m_bStates[0] = in[kk];
		out[kk] = 0.0;	
		for (unsigned int bb = 0; bb < m_b.size(); bb++)
			out[kk] += m_b[bb] * m_bStates[bb];
		
		for (unsigned int aa = 1; aa < m_a.size(); aa++)
			out[kk] -= m_a[aa] * m_aStates[aa];

		m_aStates[0] = out[kk];

		// shift states
		// std::rotate(m_bStates.begin(), m_bStates.end() - 1, m_bStates.end());
		std::move_backward(m_bStates.begin(), m_bStates.end() - 1, m_bStates.end());
		// std::rotate(m_aStates.begin(), m_aStates.end() - 1, m_aStates.end());
		std::move_backward(m_aStates.begin(), m_aStates.end() - 1, m_aStates.end());
	}

	return 0;
}

int GeneralIR::processDataTV(std::vector<double>& in, std::vector<double>& out)
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
			double newOut = 0.0;
			double oldOut = 0.0;

			m_bStates[0] = in[kk];
			for (unsigned int bb = 0; bb < m_b.size(); bb++)
			{
				newOut += m_b[bb] * m_bStates[bb];
				oldOut += m_bOld[bb] * m_bStates[bb];
			}


			for (unsigned int aa = 1; aa < m_a.size(); aa++)
			{
				newOut -= m_a[aa] * m_aStates[aa];
				oldOut -= m_aOld[aa] * m_aStatesOld[aa];
			}
			

			m_aStates[0] = newOut;
			m_aStatesOld[0] = oldOut;

			// shift states
			// std::rotate(m_bStates.begin(), m_bStates.end() - 1, m_bStates.end());
			std::move_backward(m_bStates.begin(), m_bStates.end() - 1, m_bStates.end());
			// std::rotate(m_aStates.begin(), m_aStates.end() - 1, m_aStates.end());
			std::move_backward(m_aStates.begin(), m_aStates.end() - 1, m_aStates.end());
			std::move_backward(m_aStatesOld.begin(), m_aStatesOld.end() - 1, m_aStatesOld.end());
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

int GeneralIR::setXFadeSamples(int nrofsamples)
{
	m_xFadeTimeSamples = nrofsamples;
	m_xFadeCounter = 0;
	m_StepSize = 1.0 / (m_xFadeTimeSamples - 1);
	m_CrossGain = 0.0;
	return 0;
}

