/**
 * @file GeneralIR.h
 * @author J. Bitzer (Jade Hochschule)
 * @brief a general IIR filter with crossfade to implment time-variant filter sweeps
 * ISSUE: the impmentation is faulty for changing orders, do not change filter order!!
 * @version 0.1
 * @date 2021-01-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* ToDo:
	0. repair for arbitrary filter order change (always use the longer filter 
		and add zeros to the shorter filter)
	1. reset function is misssing
	2. special faster implementation for SOS
	3. implementation of higher order filters withs SOS calculation
*/
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


