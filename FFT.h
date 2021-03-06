#pragma once

/*
    functions
        constructor: n = block size, must be a power of two

    magnitude spectral density
        magnitude(input[0 ... n-1], output[0 ... n/2])

    power spectral density
        power(input[0 ... n-1], output[0 ... n/2])

    fft for real-valued data
        fft(input[0 ... n-1], real[0 ... n/2], imag[0 ... n/2])

    inverse fft for real-valued data, including scaling by 1/n
        ifft(real[0 ... n/2], imag[0 ... n/2], output[0 ... n-1])
*/
#include <vector>
class spectrum
{
    public:
        spectrum(int n = 0);
        void setFFTSize(int n = 0);

        void magnitude(double *input, double *output);
        void power(double *input, double *output);
        void fft(double *input, double *real, double *imag);
        void ifft(double *real, double *imag, double *output);

        void magnitude(float *input, float *output);
        void power(float *input, float *output);
        void fft(float *input, float *real, float *imag);
        void ifft(float *real, float *imag, float *output);
        void power(float *input, std::vector<float>& output);

        int  get_size(void);
        virtual ~spectrum(void);

    protected:
        int nfft;
        int br_size;
        int *bitrev_table;
        double *cos_table;
        double *sin_table;
        double *cos2_table;
        double *real_part;
        double *imag_part;
        void fftc(double *real, double *imag);
        void fftr_post(double *real, double *imag);
        void ifftr_pre(double *real, double *imag);
        int  bitreverse(int num, int width);
        int  ilog2(int iarg);
        void fftshift(double *x, int n);
};
