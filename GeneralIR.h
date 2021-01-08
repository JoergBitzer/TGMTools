#pragma once
#include <vector>
class GeneralIR
{
public:
	GeneralIR();
	GeneralIR(std::vector<double> b, std::vector<double> a);
	int setCoeffs(std::vector<double> b, std::vector<double> a);
	
	int processData(std::vector<double>& in, std::vector<double>& out);
	int processDataTV(std::vector<double>& in, std::vector<double>& out);
	int setXFadeSamples(int nrofsamples);

private:
	std::vector<double> m_b;
	std::vector<double> m_a;
	std::vector<double> m_bStates;
	std::vector<double> m_aStates;
	std::vector<double> m_bOld;
	std::vector<double> m_aOld;
	std::vector<double> m_aStatesOld;

	bool m_newCoeffs;
	int m_xFadeCounter;
	int m_xFadeTimeSamples;
	double m_CrossGain;
	double m_StepSize;
};


