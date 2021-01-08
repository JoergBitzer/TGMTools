#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

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
}

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
}


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
