/* ----------------------------------------------------
// This class implements the following:
// FDLS Design with minimum phase for a lowpass / highpass filter of arbritary order
// Schmidt, T., Bitzer, J. (2010, May). Digital equalization filter: New solution to the frequency response near Nyquist and evaluation by listening tests. In Audio Engineering Society Convention 128, preprint 7964
// and https://dspblog.audio-dsp.de/post/iirlowpassfilterarbitrary/
// Author: J.Bitzer@TGM@Jade Hochschule
// Version 1.0 04.04.2020 tested 
// This software needs the Eigen Library for matrix computation
// http://eigen.tuxfamily.org/index.php?title=Main_Page
-------------------------------------------------------*/
/*ToDO:
1.) SOS Seperation using roots (see https://en.wikipedia.org/wiki/Companion_matrix)
    the complex roots are the eigenvalues of the Companionmatrix of the polynomial
*/

#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
using namespace Eigen;

class FreeOrderLowHighpassFilter
{
public:
	enum FilterType
	{
		lowpass = 0,
		highpass,
		nroffiltertypes
	};
	FreeOrderLowHighpassFilter();
	~FreeOrderLowHighpassFilter();
	int setOutputFilterOrder(int orderNom, int orderDen);
	int getCoefficients(std::vector<double>& bTrans, std::vector<double>& aRecursive, double samplerate, double cutoff, double order, FilterType filtertype);

private:
	int m_Len;
	unsigned int m_orderNom;
	unsigned int m_orderDen;
	VectorXd m_magH;
	VectorXd m_freqvek;
	VectorXd m_magH_log;
	VectorXcd m_hilbert;
	FFT<double> fft;
	VectorXd m_phase;
	VectorXd m_phase_end;
	VectorXd m_y;
	MatrixXd m_X;
	VectorXd m_coeffs;
};

