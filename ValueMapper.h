/**
 * @file ValueMapper.h
 * @author J.bitzer @ TGM @ Jade Hochschule
 * @brief This helper class interpolates between two points with a power function (1 = linear interpolation)
 * @version 0.1
 * @date 2021-05-22
 * 
 * @copyright Copyright (c) 2021 (Licence is BSD 3-clause)
 * 
 */
#pragma once
#include <cmath>
/* ToDO
1) write documentation
//*/
class ValueMapper
{
public:
    ValueMapper()
    {m_x1 = 0.0;m_x2 = 1.0;m_y1 = 0.0;m_y2 = 1.0;m_form = 1.0;computex();computey();};
    ValueMapper(double x1, double y1, double x2, double y2, double form = 1.0)
    {m_x1 = x1;m_x2 = x2;m_y1 = y1;m_y2 = y2;m_form = form;computex();computey();};
    void setx1(double x1) {m_x1 = x1; computex();};
    void setx2(double x2) {m_x2 = x2; computex();};
    void sety1(double y1) {m_y1 = y1; computey();};
    void sety2(double y2) {m_y2 = y2; computey();};
    void setForm(double newForm){m_form = newForm;};

    inline double getValue (double xin){if (xin > m_x2) xin = m_x2;
        return ( m_diffy*pow(m_inversediffx*(xin-m_x1),m_form) + m_y1);
    };
private:
    inline void computex(){m_inversediffx = 1.0/(m_x2 -m_x1);};
    inline void computey(){m_diffy = (m_y2-m_y1); };
    double m_x1;
    double m_y1;
    double m_x2;
    double m_y2;
    double m_form;
    
    double m_inversediffx;
    double m_diffy;
};