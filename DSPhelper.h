/**
 * @file DSPhelper.h
 * @author J. Bitzer @ Jade Hochschule
 * @brief Helper for filter design from analog to digital domain (bilinear, Lp->LP, LP->HP)
 * Only for first and second order filters
 * @version 0.1
 * @date 2021-01-11
 * 
 * @copyright Copyright (c) 2021  (BSD Licence)
 * 
 */
#pragma once

#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <vector>
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif
/**
 * @brief bilinearTransform transforms analog coeficients to their digital equivalents.
 * This function works for first and second order section filters only
 * @param d : the numerator of the analog systemfunction H(s) 
 * @param c : the denominator of the analog systemfunction H(s)
 * @param b : output var: the numerator of the resulting digital systemfunction H(z) 
 * @param a : outpu var: the denominator of the analog systemfunction H(s)
 * @param sampleRate : the samplerate in Hz for the design
 * @param fcut : the cutoff frequency in Hz of the filter 
 * @return int : error code : -1 and -2 if the analog filters have the wrong length
 *                            0 = OK
 */
int bilinearTransform(const std::vector<double>& d, const std::vector<double>& c,
	std::vector<double>& b, std::vector<double>& a,
	const double sampleRate, const double fcut)
{
	
	if (d.size() > 3 || c.size() > 3)
		return -1;

	if (d.size() != c.size())
		return -2;

	const double K = 2.0 * M_PI * fcut / (tan(M_PI * fcut / sampleRate));

	b.resize(c.size());
	a.resize(c.size());
	// First order
	if (d.size() == 2)
	{
		const double Norm = 1.0 / (c[0] * K + c[1]);

		b[0] = (K * d[0] + d[1]) * Norm;
		b[1] = (-K * d[0] + d[1]) * Norm;
		a[1] = (-K * c[0] + c[1]) * Norm;
		a[0] = 1.0;
	}
	// second order
	if (d.size() == 3)
	{
		const double Norm = 1.0 / (c[0] * K * K + c[1] * K + c[2]);
		b[0] = (d[0]*K*K + d[1]*K +d[2]) * Norm;
		b[1] = (2*d[2] - 2*d[0]*K*K) * Norm;
		b[2] = (d[0]*K*K -d[1]*K + d[2]) * Norm;
		a[1] = (2*c[2] -2*c[0]*K*K) * Norm;	
		a[2] = (c[0]*K*K - c[1]*K + c[2]) * Norm;
		a[0] = 1.0;
	}
	return 0;
}
/**
 * @brief denormalizing transfrom for lowpass design (first and second order only)
 * 
 * @param dnorm : normalized numerator input analog coeffs (cutoff omega = 1)
 * @param cnorm : normalized denominator input analog coeffs (cutoff omega = 1)
 * @param d : output var: the denormalized coeffs
 * @param c  output var: the denormalized coeffs
 * @param fcut : the cutoff frequenvy in Hz
 * @return int ; -1 and -2 if the length is wrong,  0 = OK
 */
int LowpassLowpassTransform(const std::vector<double>& dnorm, const std::vector<double>& cnorm,
	std::vector<double>& d, std::vector<double>& c, const double fcut)
{
	double om = 2.0 * M_PI * fcut;
	if (dnorm.size() > 3 || cnorm.size() > 3)
		return -1;

	if (dnorm.size() != cnorm.size())
		return -2;

	c.resize(cnorm.size());
	d.resize(dnorm.size());



	if (dnorm.size() == 2)
	{
		d[0] = dnorm[0];
		d[1] = dnorm[1] * om;
		c[0] = cnorm[0];
		c[1] = cnorm[1] * om;
	}
	if (dnorm.size() == 3)
	{
		d[0] = dnorm[0];
		d[1] = dnorm[1] * om;
		d[2] = dnorm[2] * om * om;
		c[0] = cnorm[0];
		c[1] = cnorm[1] * om;
		c[2] = cnorm[2] * om * om;
	}
	return 0;
}
/**
 * @brief denormalizing transfrom for highpass design (first and second order only)
 * 
 * @param dnorm : normalized numerator input analog coeffs (cutoff omega = 1)
 * @param cnorm : normalized denominator input analog coeffs (cutoff omega = 1)
 * @param d : output var: the denormalized coeffs
 * @param c  output var: the denormalized coeffs
 * @param fcut : the cutoff frequenvy in Hz
 * @return int ; -1 and -2 if the length is wrong,  0 = OK
 */
int LowpassHighpassTransform(const std::vector<double>& dnorm, const std::vector<double>& cnorm,
	std::vector<double>& d, std::vector<double>& c, const double fcut)
{
	double om = 2.0 * M_PI * fcut;
	if (dnorm.size() > 3 || cnorm.size() > 3)
		return -1;

	if (dnorm.size() != cnorm.size())
		return -2;

	c.resize(cnorm.size());
	d.resize(dnorm.size());

	if (dnorm.size() == 2)
	{
		d[0] = dnorm[1];
		d[1] = dnorm[0] * om;
		c[0] = cnorm[1];
		c[1] = cnorm[0] * om;
	}
	if (dnorm.size() == 3)
	{
		d[0] = dnorm[2];
		d[1] = dnorm[1] * om;
		d[2] = dnorm[0] * om * om;
		c[0] = cnorm[2];
		c[1] = cnorm[1] * om;
		c[2] = cnorm[0] * om * om;
	}
}

/*
Testcode for a second order butterworth filter 
	std::vector <double> b;
	std::vector <double> a;
	std::vector <double> d;
	std::vector <double> c;
	std::vector <double> dnorm;
	std::vector <double> cnorm;

	dnorm.resize(3);
	cnorm.resize(3);

	double fcut = 1000.0;
	double fs = 44100.0;

	dnorm[0] = 0.0;
	dnorm[1] = 0.0;
	dnorm[2] = 1.0;
	cnorm[0] = 1.0;
	cnorm[1] = sqrt(2);
	cnorm[2] = 1.0;

	LowpassLowpassTransform(dnorm, cnorm, d, c, fcut);
	bilinearTransform(d, c, b, a, fs, fcut);


*/
