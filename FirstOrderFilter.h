#pragma once

#define PI_M 3.141592653589793

#include <vector>
class FirstOrderFilter
{
public:
	enum class FilterDesign
	{
		lowpassSmooth,
		lowpassButter,
		lowShelv,
		highpassSmooth,
		highpassButter,
		highShelv,
		none
	};
	FirstOrderFilter() :m_fs(44100.0), m_cutoff(1000.0), m_gain_db(0.0),
		m_design(FilterDesign::none) {
		computeCoeffs();
		reset();
	};
	FirstOrderFilter(double fcut, double fs, FilterDesign design)
	:m_cutoff(fcut),m_fs(fs),m_design(design)
	{
		m_gain_db = 0.0;
		computeCoeffs();
		reset();
	};
	FirstOrderFilter(double fcut, double fs, double gain_dB, FilterDesign design)
		:m_cutoff(fcut), m_fs(fs), m_gain_db(gain_dB), m_design(design)
	{
		computeCoeffs();
		reset();
	};
	void setSamplerate(double samplerate)
	{
		m_fs = samplerate; computeCoeffs();
		reset();
	};
	void setCutoff(double cutoff)
	{
		m_cutoff = cutoff;
		computeCoeffs();
	};
	void setDesignroutine(FilterDesign design)
	{
		m_design = design;
		computeCoeffs();
	};
	void setGainIndB(double gain_dB)
	{
		m_gain_db = gain_dB;
		computeCoeffs();
	};
	int processData(std::vector<double>& data)
	{
		for (auto kk = 0u; kk < data.size(); ++kk)
		{
			// DF 2 filter
			double instate = data[kk] - m_a1 * m_state;
			data[kk] = instate * m_b0 + m_state * m_b1;
			m_state = instate;
		}
		return 0;
	};
	inline float processOneSample(float data)
	{
		// DF 2 filter
		double instate = data - m_a1 * m_state;
		float Temp =  instate * m_b0 + m_state * m_b1;
		m_state = instate;
		return Temp;
	};
	inline double processOneSample(double data)
	{
		// DF 2 filter
		double instate = data - m_a1 * m_state;
		double Temp = instate * m_b0 + m_state * m_b1;
		m_state = instate;
		return Temp;
	};
private:
	double m_b0, m_b1, m_a1;
	double m_state;
	double m_fs, m_cutoff;
	double m_gain_db;
	FilterDesign m_design;
	void computeCoeffs() // music dsp
	{
		double fcut;
		double w;
		double Norm;
		double om;
		double A; // RBJ A
		switch (m_design)
		{
		case FilterDesign::none:
			m_b0 = 1.0;
			m_a1 = m_b1 = 0.0;
			break;
		case FilterDesign::lowpassButter:
			fcut = tan(PI_M * m_cutoff / m_fs) *2.0* m_fs ;
			w = 2.0 * m_fs;
			Norm = 1.0 / (fcut + w);
			m_a1 = -(w - fcut)*Norm;
			m_b0 = m_b1 = fcut * Norm;
			break;
		case FilterDesign::highpassButter:
			fcut = tan(PI_M * m_cutoff / m_fs) * 2.0 * m_fs;
			w = 2.0 * m_fs;
			Norm = 1.0 / (fcut + w);
			m_b0 = w * Norm;
			m_b1 = -m_b0;
			m_a1 = -(w - fcut) * Norm;
			break;
		case FilterDesign::lowpassSmooth:
			om = 2.0 * PI_M * m_cutoff / m_fs;
			m_b1 = 0.0;
			m_a1 = (2.0 - cos(om)) - sqrt((2 - cos(om)) * (2 - cos(om)) - 1.0);
			m_b0 = (1.0 - m_a1);
			m_a1 *= -1.0;
			break;
		case FilterDesign::highpassSmooth:
			om = 2.0 * PI_M * m_cutoff / m_fs;
			m_b1 = 0.0;
			m_a1 = ((2.0 + cos(om)) - sqrt((2 + cos(om)) * (2 + cos(om)) - 1.0));
			m_b0 = m_a1 - 1.0;
			break;
		case FilterDesign::lowShelv:
			A = pow(10.0, m_gain_db / 40.0); // 40 see RBJ cookbook
			fcut = tan(PI_M * m_cutoff / m_fs) * 2.0 * m_fs;
			w = 2.0 * m_fs;
			Norm = 1.0 / (fcut + w*A);
			m_b0 = A * (A * fcut + w) * Norm;
			m_b1 = - A* (w-A*fcut)*Norm;
			m_a1 = -(A*w - fcut) * Norm;
			break;
		case FilterDesign::highShelv:
			A = pow(10.0, m_gain_db / 40.0); // 40 see RBJ cookbook
			fcut = tan(PI_M * m_cutoff / m_fs) * 2.0 * m_fs;
			w = 2.0 * m_fs;
			Norm = 1.0 / (A*fcut + w);
			m_b0 = A * (fcut + A*w) * Norm;
			m_b1 = -A * (A*w -  fcut) * Norm;
			m_a1 = -(w - A*fcut) * Norm;
			break;

		}

	};
	void reset()
	{
		m_state = 0.0;
	};
};

