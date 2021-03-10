#include <math.h>
#include <stdlib.h>

#include "FFT.h"

#define M_PI 3.14159265358979323846

/*-----------------------------------------------------------------------*\
| An FFT-Class including magnitude spectral density and                   |
| power spectral density                                                  |
|                                                                         |
| Author: (c) Uwe Simmer, Fachhochschule Oldenburg / Ostfriesland /       |
| Wilhelmshaven, Institut f�r H�rtechnik und Audiologie (FH OOW / IHA)    |
| License: see End of File                                                |
|                                                                         |
| Version 1.0, Date 07. Oct. 2005:  initial version                       |
| Version 1.1, Date 20. Feb. 2006:  added float interface                 |
\*-----------------------------------------------------------------------*/

spectrum::spectrum(int n)
{

    setFFTSize(n);
}

void spectrum::setFFTSize(int n)
{
    int i, j, m;

    nfft = 0;

    // n must be >= 2
    if (n < 2)
    {
        return;
    }

    // n must be a power of 2
    if (!ilog2(n))
    {
        return;
    }
    // delete mem
     
//    if (bitrev_table != nullptr)
//        delete[] bitrev_table;


    nfft = n / 2;

    // size of the bitreverse-table
    br_size = nfft/2 - (1 << ((ilog2(nfft) - 1) / 2));

    // allocate memory for the bitreverse-table
    bitrev_table.resize(2*br_size);

    // compute bitreverse table
    m = 0;
    for (i=0; i<nfft; i++)
    {
        j = bitreverse(i, ilog2(nfft));

        if (j > i)
        {
            bitrev_table[m++] = i;
            bitrev_table[m++] = j;
        }
    }

    // allocate memory for tables of fftc
    sin_table.resize(nfft/2);
    cos_table.resize(nfft/2);

    // compute tables of fftc
    for (i=0; i<nfft/2; i++)
    {
        cos_table[i] = cos(2 * i * M_PI / nfft);
        sin_table[i] = sin(2 * i * M_PI / nfft);
    }

    // allocate memory for cos-table of fftr and ifftr
    cos2_table.resize(nfft);

    // compute cos-table of real ffts
    for (i=0; i<nfft; i++)
    {
        cos2_table[i] = cos(i * M_PI / nfft / 2);
    }

    // allocate memory for real and imag part
    real_part.resize(nfft+1);
    imag_part.resize(nfft+1);
}

//-------------------------------------------------------------------------

void spectrum::magnitude(double *input, double *output)
{
    int i;

    if (nfft == 0 || input == NULL || output == NULL)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real_part[i] = input[2*i];      // copy even samples to real part
        imag_part[i] = input[2*i+1];    // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real_part, imag_part);

    // postrocessor for real fft
    fftr_post(real_part, imag_part);

    for (i=0; i<nfft+1; i++)
    {
        output[i] = sqrt(real_part[i] * real_part[i]
                       + imag_part[i] * imag_part[i]);
    }

    return;
}

//-------------------------------------------------------------------------

void spectrum::magnitude(float *input, float *output)
{
    int i;

    if (nfft == 0 || input == NULL || output == NULL)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real_part[i] = input[2*i];      // copy even samples to real part
        imag_part[i] = input[2*i+1];    // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real_part, imag_part);

    // postrocessor for real fft
    fftr_post(real_part, imag_part);

    for (i=0; i<nfft+1; i++)
    {
        output[i] = float(sqrt(real_part[i] * real_part[i]
                             + imag_part[i] * imag_part[i]));
    }

    return;
}

//-------------------------------------------------------------------------

void spectrum::power(double *input, double *output)
{
    int i;

    if (nfft == 0 || input == NULL || output == NULL)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real_part[i] = input[2*i];      // copy even samples to real part
        imag_part[i] = input[2*i+1];    // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real_part, imag_part);

    // postrocessor for real fft
    fftr_post(real_part, imag_part);

    for (i=0; i<nfft+1; i++)
    {
        output[i] = real_part[i] * real_part[i]
                  + imag_part[i] * imag_part[i];
    }

    return;
}



//-------------------------------------------------------------------------

void spectrum::power(float *input, float *output)
{
    int i;

    if (nfft == 0 || input == NULL || output == NULL)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real_part[i] = input[2*i];      // copy even samples to real part
        imag_part[i] = input[2*i+1];    // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real_part, imag_part);

    // postrocessor for real fft
    fftr_post(real_part, imag_part);

    for (i=0; i<nfft+1; i++)
    {
        output[i] = float(real_part[i] * real_part[i]
                        + imag_part[i] * imag_part[i]);
    }

    return;
}

void spectrum::power(float *input, std::vector<float>& output)
{
    int i;

    if (nfft == 0 || input == NULL )
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real_part[i] = input[2*i];      // copy even samples to real part
        imag_part[i] = input[2*i+1];    // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real_part, imag_part);

    // postrocessor for real fft
    fftr_post(real_part, imag_part);

    for (i=0; i<nfft+1; i++)
    {
        output[i] = float(real_part[i] * real_part[i]
                        + imag_part[i] * imag_part[i]);
    }

    return;
}

//-------------------------------------------------------------------------

void spectrum::fft(double *input, double *real, double *imag)
{
    int i;

    if (nfft == 0)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real[i] = input[2*i];       // copy even samples to real part
        imag[i] = input[2*i+1];     // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real, imag);

    // postrocessor for real fft
    fftr_post(real, imag);
}
void spectrum::fft(std::vector<double>& input, std::vector<double>&real,std::vector<double>&imag)
{
    int i;

    if (nfft == 0)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real[i] = input[2*i];       // copy even samples to real part
        imag[i] = input[2*i+1];     // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real, imag);

    // postrocessor for real fft
    fftr_post(real, imag);
}

//-------------------------------------------------------------------------

void spectrum::fft(float *input, float *real, float *imag)
{
    int i;

    if (nfft == 0)
    {
        return;
    }

    for (i=0; i<nfft; i++)
    {
        real_part[i] = input[2*i];      // copy even samples to real part
        imag_part[i] = input[2*i+1];    // copy odd  samples to imag part
    }

    // complex half length fft
    fftc(real_part, imag_part);

    // postrocessor for real fft
    fftr_post(real_part, imag_part);

    for (i=0; i<nfft+1; i++)
    {
        real[i] = float(real_part[i]);  // convert real part to float
        imag[i] = float(imag_part[i]);  // convert imag part to float
    }
}

//-------------------------------------------------------------------------

void spectrum::ifft(double *real, double *imag, double *output)
{
    int i;
    double norm;

    if (nfft == 0 || output == NULL)
    {
        return;
    }

    // save real part
    if (real != NULL)
    {
        for (i=0; i<nfft+1; i++)
        {
            real_part[i] = real[i];
        }
    }
    else
    {
        for (i=0; i<nfft+1; i++)
        {
            real_part[i] = 0.;
        }
    }

    // save imag part
    if (imag != NULL)
    {
        for (i=0; i<nfft+1; i++)
        {
            imag_part[i] = imag[i];
        }
    }
    else
    {
        for (i=0; i<nfft+1; i++)
        {
            imag_part[i] = 0;
        }
    }

    // preprocessor for real inverse fft
    ifftr_pre(real_part, imag_part);

    // complex half length fft
    fftc(real_part, imag_part);

    norm = 1. / double(nfft);

    for (i=0; i<nfft; i++)
    {
        output[2*i] = real_part[i] * norm;
        output[2*i+1] = imag_part[i] * norm;
    }
}

//-------------------------------------------------------------------------

void spectrum::ifft(float *real, float *imag, float *output)
{
    int i;
    double norm;

    if (nfft == 0 || output == NULL)
    {
        return;
    }

    // save real part
    if (real != NULL)
    {
        for (i=0; i<nfft+1; i++)
        {
            real_part[i] = real[i];
        }
    }
    else
    {
        for (i=0; i<nfft+1; i++)
        {
            real_part[i] = 0.;
        }
    }

    // save imag part
    if (imag != NULL)
    {
        for (i=0; i<nfft+1; i++)
        {
            imag_part[i] = imag[i];
        }
    }
    else
    {
        for (i=0; i<nfft+1; i++)
        {
            imag_part[i] = 0;
        }
    }

    // preprocessor for real inverse fft
    ifftr_pre(real_part, imag_part);

    // complex half length fft
    fftc(real_part, imag_part);

    norm = 1. / double(nfft);

    for (i=0; i<nfft; i++)
    {
        output[2*i]   = float(real_part[i] * norm);
        output[2*i+1] = float(imag_part[i] * norm);
    }
}

//-------------------------------------------------------------------------

void spectrum::fftc(double *real, double *imag)
{
    int    i, ig, isp, j, k, l, m;
    double rtemp, itemp, co, si;

    if (nfft == 0 || real == NULL || imag == NULL)
    {
        return;
    }

    // bitreverse section
    for (k=0; k<br_size; k++)
    {
        i = bitrev_table[2*k];
        j = bitrev_table[2*k+1];

        rtemp   = real[j];
        real[j] = real[i];
        real[i] = rtemp;

        itemp   = imag[j];
        imag[j] = imag[i];
        imag[i] = itemp;
    }

    i = 0;
    j = 1;
    m = nfft >> 1;

    // first stage
    for (ig=0; ig<m; ig++)
    {
        rtemp = real[j];
        itemp = imag[j];
        real[j] = real[i] - rtemp;
        real[i] = real[i] + rtemp;
        imag[j] = imag[i] - itemp;
        imag[i] = imag[i] + itemp;
        i = i + 2;
        j = j + 2;
    }

    isp = 2;

    // log2(n)-1 stages
    for (m = nfft >> 2; m > 0; m >>= 1)
    {
        i = 0;
        j = isp;

        for (ig=0; ig<m; ig++)
        {
            k = 0;
            for (l=0; l<isp; l++)
            {
                co = cos_table[k];
                si = sin_table[k];
                k += m;

                // Danielson-Lanczos formula
                rtemp = real[j] * co + imag[j] * si;
                itemp = imag[j] * co - real[j] * si;
                real[j] = real[i] - rtemp;
                real[i] = real[i] + rtemp;
                imag[j] = imag[i] - itemp;
                imag[i] = imag[i] + itemp;

                i++;
                j++;
            }

            i = i + isp;
            j = j + isp;
        }

        isp <<= 1;
    }
}

//-------------------------------------------------------------------------

void spectrum::fftr_post(double *real, double *imag)
{
    int    i, j;
    double x, y, rs, is, rd, id, rp, ip, ci, cj;

    if (nfft == 0 || real == NULL || imag == NULL)
    {
        return;
    }

    x = real[0];
    y = imag[0];
    real[0] = x + y;
    real[nfft] = x - y;
    imag[0] = 0.0;
    imag[nfft] = 0.0;

    for (i=1; i<nfft>>1; i++)
    {
        j = nfft - i;
        ci = cos2_table[2*i];
        cj = cos2_table[nfft-2*i];

        rs = (real[i] + real[j]) * 0.5;
        is = (imag[i] + imag[j]) * 0.5;
        rd = (real[j] - real[i]) * 0.5;
        id = (imag[i] - imag[j]) * 0.5;

        rp = is * ci + rd * cj;
        real[i] = rp + rs;
        real[j] = rs - rp;

        ip = rd * ci - is * cj;
        imag[i] = ip + id;
        imag[j] = ip - id;
    }

    real[nfft/2] =  real[nfft/2];
    imag[nfft/2] = -imag[nfft/2];
}

//-------------------------------------------------------------------------

void spectrum::ifftr_pre(double *real, double *imag)
{
    int    i, j;
    double x, y, rs, is, rd, id, rp, ip, ci, cj;

    if (nfft == 0 || real == NULL || imag == NULL)
    {
        return;
    }

    x = real[0];
    y = real[nfft];
    real[0] = (x + y) * 0.5;
    imag[0] = (x - y) * 0.5;

    for (i=1; i<nfft/2; i++)
    {
        j = nfft - i;
        ci = cos2_table[2*i];
        cj = cos2_table[nfft-2*i];

        rs = (real[i] + real[j]) * 0.5;
        rd = (real[i] - real[j]) * 0.5;
        is = (imag[i] + imag[j]) * 0.5;
        id = (imag[i] - imag[j]) * 0.5;

        rp = is * ci + rd * cj;
        real[i] = rp + rs;
        real[j] = rs - rp;

        ip = rd * ci - is * cj;
        imag[i] = ip - id;
        imag[j] = ip + id;
    }

    real[nfft/2] =  real[nfft/2];
    imag[nfft/2] = -imag[nfft/2];
}
void spectrum::fftc(std::vector<double>& real, std::vector<double>& imag)
{
    int    i, ig, isp, j, k, l, m;
    double rtemp, itemp, co, si;

    if (nfft == 0 )
    {
        return;
    }

    // bitreverse section
    for (k=0; k<br_size; k++)
    {
        i = bitrev_table[2*k];
        j = bitrev_table[2*k+1];

        rtemp   = real[j];
        real[j] = real[i];
        real[i] = rtemp;

        itemp   = imag[j];
        imag[j] = imag[i];
        imag[i] = itemp;
    }

    i = 0;
    j = 1;
    m = nfft >> 1;

    // first stage
    for (ig=0; ig<m; ig++)
    {
        rtemp = real[j];
        itemp = imag[j];
        real[j] = real[i] - rtemp;
        real[i] = real[i] + rtemp;
        imag[j] = imag[i] - itemp;
        imag[i] = imag[i] + itemp;
        i = i + 2;
        j = j + 2;
    }

    isp = 2;

    // log2(n)-1 stages
    for (m = nfft >> 2; m > 0; m >>= 1)
    {
        i = 0;
        j = isp;

        for (ig=0; ig<m; ig++)
        {
            k = 0;
            for (l=0; l<isp; l++)
            {
                co = cos_table[k];
                si = sin_table[k];
                k += m;

                // Danielson-Lanczos formula
                rtemp = real[j] * co + imag[j] * si;
                itemp = imag[j] * co - real[j] * si;
                real[j] = real[i] - rtemp;
                real[i] = real[i] + rtemp;
                imag[j] = imag[i] - itemp;
                imag[i] = imag[i] + itemp;

                i++;
                j++;
            }

            i = i + isp;
            j = j + isp;
        }

        isp <<= 1;
    }
}

//-------------------------------------------------------------------------

void spectrum::fftr_post(std::vector<double>& real, std::vector<double>& imag)
{
    int    i, j;
    double x, y, rs, is, rd, id, rp, ip, ci, cj;

    if (nfft == 0 )
    {
        return;
    }

    x = real[0];
    y = imag[0];
    real[0] = x + y;
    real[nfft] = x - y;
    imag[0] = 0.0;
    imag[nfft] = 0.0;

    for (i=1; i<nfft>>1; i++)
    {
        j = nfft - i;
        ci = cos2_table[2*i];
        cj = cos2_table[nfft-2*i];

        rs = (real[i] + real[j]) * 0.5;
        is = (imag[i] + imag[j]) * 0.5;
        rd = (real[j] - real[i]) * 0.5;
        id = (imag[i] - imag[j]) * 0.5;

        rp = is * ci + rd * cj;
        real[i] = rp + rs;
        real[j] = rs - rp;

        ip = rd * ci - is * cj;
        imag[i] = ip + id;
        imag[j] = ip - id;
    }

    real[nfft/2] =  real[nfft/2];
    imag[nfft/2] = -imag[nfft/2];
}

//-------------------------------------------------------------------------

void spectrum::ifftr_pre(std::vector<double>& real, std::vector<double>& imag)
{
    int    i, j;
    double x, y, rs, is, rd, id, rp, ip, ci, cj;

    if (nfft == 0)
    {
        return;
    }

    x = real[0];
    y = real[nfft];
    real[0] = (x + y) * 0.5;
    imag[0] = (x - y) * 0.5;

    for (i=1; i<nfft/2; i++)
    {
        j = nfft - i;
        ci = cos2_table[2*i];
        cj = cos2_table[nfft-2*i];

        rs = (real[i] + real[j]) * 0.5;
        rd = (real[i] - real[j]) * 0.5;
        is = (imag[i] + imag[j]) * 0.5;
        id = (imag[i] - imag[j]) * 0.5;

        rp = is * ci + rd * cj;
        real[i] = rp + rs;
        real[j] = rs - rp;

        ip = rd * ci - is * cj;
        imag[i] = ip - id;
        imag[j] = ip + id;
    }

    real[nfft/2] =  real[nfft/2];
    imag[nfft/2] = -imag[nfft/2];
}


//-------------------------------------------------------------------------

int spectrum::bitreverse(int input, int width)
{
    int i, inp, rev;

    rev = 0;
    inp = input;

    for (i=0; i<width; i++)
    {
        rev = (rev << 1) | (inp & 1);
        inp = inp >> 1;
    }
 
    return(rev);
}

//-------------------------------------------------------------------------

int spectrum::ilog2(int iarg)
{
    int i, n=0;

    for (i=1; i<iarg; i*=2)
        n++;

    if (i != iarg)
        return(0);

    return(n);
}

//-------------------------------------------------------------------------

int spectrum::get_size(void)
{
    return(2*nfft);
}

//-------------------------------------------------------------------------

spectrum::~spectrum(void)
{

}

//------------------------------------------------------------------------//
// Copyright (c) 2005-2006 Uwe Simmer                                     //
// Institute of Hearing Technology and Audiology (IHA), University of     //
// Applied Sciences Oldenburg / Ostfriesland / Wilhelmshaven (FH OOW)     //
// Ofener Strasse 16-19, 26121 Oldenburg, Germany                         //
//                                                                        //
// Permission is hereby granted, free of  charge, to any person obtaining //
// a copy  of this  software  and  associated  documentation  files  (the //
// "Software"), to  deal in the  Software without  restriction, including //
// without limitation the  rights to  use, copy, modify,  merge, publish, //
// distribute,  sublicense, and / or sell copies of  the Software, and to //
// permit  persons to whom  the Software  is furnished to  do so, subject //
// to the following conditions:                                           //
//   1) The above  copyright notice and  this permission notice  shall be //
//      included in all copies or substantial portions of the Software.   //
//                                                                        //
// THE  SOFTWARE IS  PROVIDED  "AS IS",  WITHOUT  WARRANTY  OF ANY  KIND, //
// EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE  WARRANTIES //
// OF   MERCHANTABILITY,   FITNESS   FOR   A   PARTICULAR   PURPOSE   AND //
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE //
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION //
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN  CONNECTION //
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        //
//                                                                        //
//------------------------------------------------------------------------//
