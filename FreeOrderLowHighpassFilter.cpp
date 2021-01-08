#include "FreeOrderLowHighpassFilter.h"

FreeOrderLowHighpassFilter::FreeOrderLowHighpassFilter()
{
    m_Len = 4*256;
    m_magH.resize(m_Len / 2 + 1, 1);
    m_freqvek.resize(m_Len / 2 + 1, 1);
    m_magH_log.resize(m_Len, 1);
    m_hilbert.resize(m_Len, 1);
    m_phase.resize(m_Len, 1);
    m_phase_end.resize(m_Len / 2 + 1, 1);
    m_y.resize(m_Len / 2 + 1, 1);
}

FreeOrderLowHighpassFilter::~FreeOrderLowHighpassFilter()
{
}

int FreeOrderLowHighpassFilter::setOutputFilterOrder(int orderNom, int orderDen)
{
    m_orderDen = orderDen;
    m_orderNom = orderNom;
    m_X.resize(m_Len / 2 + 1, m_orderNom + m_orderDen + 1);
    m_coeffs.resize(m_orderNom + m_orderDen+1);
    return 0;
}

int FreeOrderLowHighpassFilter::getCoefficients(std::vector<double>& bTrans, std::vector<double>& aRecursive, double samplerate, double cutoff, double order, FilterType filtertype)
{
    // generate butterworth filter TF of arbitrary order
 
    double C = 1.0;
    for (int kk = 0; kk < m_Len / 2 + 1; kk++)
    {
        double analfreq = double(kk) / (m_Len / 2) * samplerate * 0.5;
        std::complex<double> jom(0.0, analfreq / cutoff);
        if (filtertype == FilterType::lowpass)
            m_magH[kk] = sqrt(abs(1.0 / (1.0 + C * C * pow(jom, order) * pow(-jom, order))));
        else
            m_magH[kk] = sqrt(abs(1.0 / (1.0 + C * C * 1.0 / (pow(jom, order) + 0.000000001) * 1.0 / (pow(-jom, order) + 0.00000001))));

        m_freqvek[kk] = analfreq;
    }

    // hilbert transform to get the minimum phase 
    // https://dsp.stackexchange.com/questions/42917/hilbert-transformer-and-minimum-phase

    for (int kk = 0; kk < m_Len / 2 + 1; kk++)
    {
        m_magH_log[kk] = log(m_magH[kk]);
        if ((kk > 0)& (kk < (m_Len / 2)))
            m_magH_log[m_Len - kk] = log(m_magH[kk]);
    }

    m_hilbert = fft.fwd(m_magH_log);
    m_hilbert[0] = 0.0;
    m_hilbert[m_Len / 2] = 0.0;
    std::complex<double> j(0.0, 1.0);
    
    for (int kk = 1; kk < m_Len / 2; kk++)
    {
        m_hilbert[kk] *= -j;
        m_hilbert[m_Len - kk] *= j;
    }

    m_phase = fft.inv(m_hilbert);
    m_phase *= -1.0;
    m_phase_end = m_phase.head(m_Len / 2 + 1);
    
    // FDLS part
    for (int kk = 0; kk < m_Len / 2 + 1; kk++)
    {
        double analfreq = double(kk) / (m_Len / 2) * M_PI;
        m_y[kk] = cos(m_phase_end[kk]) * m_magH[kk];
        for (int dd = 0; dd < m_orderDen; dd++)
        {
            m_X(kk, dd) = -m_magH(kk) * cos(-(dd + 1) * analfreq + m_phase_end[kk]);
        }
        m_X(kk, m_orderDen) = 1.0;
        for (int nn = 1; nn <= m_orderNom; nn++)
        {
            m_X(kk, m_orderDen + nn) = cos(-(nn)*analfreq);
        }
    }
    m_coeffs = (m_X.transpose() * m_X).ldlt().solve(m_X.transpose() * m_y);
    
    aRecursive[0] = 1.0;
    for (unsigned int kk = 1; kk < aRecursive.size(); kk++)
    {
        if (kk < m_orderDen+1)
            aRecursive[kk] = m_coeffs[kk - 1];
    }
    for (unsigned int kk = 0; kk < bTrans.size(); kk++)
    {
        if (kk < m_orderNom + 1 )
            bTrans[kk] = m_coeffs[m_orderDen + kk];
    }

	return 0;
}
