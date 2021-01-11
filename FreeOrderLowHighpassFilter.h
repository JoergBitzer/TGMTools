/* ----------------------------------------------------
// This class implements the following:
// FDLS Design with minimum phase for a lowpass / highpass filter of arbritary order (maximally flat = Butterworth design)
// Schmidt, T., Bitzer, J. (2010, May). Digital equalization filter: New solution to the frequency response near Nyquist and evaluation by listening tests. In Audio Engineering Society Convention 128, preprint 7964
// and https://dspblog.audio-dsp.de/post/iirlowpassfilterarbitrary/
// Author: J.Bitzer@TGM@Jade Hochschule
// Version 1.0 04.04.2020 tested 
// This software needs the Eigen Library for matrix computation in the include search path
// http://eigen.tuxfamily.org/index.php?title=Main_Page
-------------------------------------------------------*/
/*ToDO:
1.) SOS Seperation for better numerical stability 
	Method: Use roots (see https://en.wikipedia.org/wiki/Companion_matrix)
    the complex roots are the eigenvalues of the Companionmatrix of the polynomial
*/

#pragma once
#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
using namespace Eigen;

/**
 * @brief This class implements the FDLS Design for a fractional order Butterworth filter (High and Lowpass)
 * the output order (the final digital coefficients) can be set and should be higher than the maximum 
 * desired order (e.g max is 2.4 ==> use 3 or 4 for the numerator and denominator).
 * Specialties: (no warp effect by bilinear tansform, fractional order)
  */
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
	/**
	 * @brief Get the Coefficients for given design parameters
	 * 
	 * @param bTrans : the transversal coefficients (typically called b) of H(z)
	 * @param aRecursive :the recursive coefficients (typically called a) of H(z)
	 * @param samplerate : the samplerate in Hz
	 * @param cutoff : the cutoff frequency (3dB point) in Hz
	 * @param order : the desired fractional order (this is the clou and the difference between this design and a standard butterworth design)
	 * @param filtertype : lowpass = 0, highpass = 1 (enum))
	 * @return int (errorcode) 0 = OK
	 */
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

