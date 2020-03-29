/*
 * 
 *  _   _   _        __   _   _   _                                         
 * | | (_) | |__    / _| (_) | | | |_    ___   _ __           _ __     __ _ 
 * | | | | | '_ \  | |_  | | | | | __|  / _ \ | '__|  _____  | '_ \   / _` |
 * | | | | | |_) | |  _| | | | | | |_  |  __/ | |    |_____| | | | | | (_| |
 * |_| |_| |_.__/  |_|   |_| |_|  \__|  \___| |_|            |_| |_|  \__, |
 *                                                                    |___/ 
 *
 * A self contained, header only library providing a set of filters 
 * written in C++ with efficiency in mind
 *
 * Version: 1.0.0
 * URL: https://github.com/ekondayan/libfilter-ng.git
 *
 * Copyright (c) 2019,2020 Emil Kondayan
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * DESCRIPTION
 * -----------
 *
 * ALGORITHM
 * ---------
 *
 * PROS
 * ----
 *
 * CONS
 * ----
 *
 * TYPE
 * ----
 * FIR
 *
 * DATA TYPES
 * ----------
 * data_t - Type of the data, the filter will work with
 * uint_t - Type of unsigned integers used. This type should be chosen carefully based on the CPU/MCU for optimal performance.
 */

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <type_traits>

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class Interpolation
    {
        public:
            struct InterpolationPoint
            {
                    data_t value;
                    float coefficient;
            };

        public:
            Interpolation(InterpolationPoint *interpolation_point, uint_t size = 0);
            data_t out();
            void in(const data_t &value);
            void reset(InterpolationPoint *interpolation_points, uint_t size);
            void reset();
            void setPoint(uint_t index, data_t real_value, data_t measured_value);

        private:
            InterpolationPoint  *m_interpolation_point;
            uint_t m_interpolation_point_size;
            data_t m_raw_value;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    Interpolation<data_t, uint_t>::Interpolation(InterpolationPoint *interpolation_point, uint_t size):
        m_interpolation_point(interpolation_point),
        m_interpolation_point_size(size),
        m_raw_value(data_t())
    {
        static_assert (std::is_unsigned_v<uint_t>, "");
    }

    template<class data_t, class uint_t>
    data_t Interpolation<data_t, uint_t>::out()
    {
        // If the m_interpolation_point_size is 0, then the coeficient is multiplied by 0 and the returned values is not interpolated
        if(!m_interpolation_point || m_interpolation_point_size == 0) return m_raw_value;

        // If m_interpolation_point_size is 1, then the coeficient is multiplied by 1 and the returned values is interpolated
        if(m_interpolation_point_size == 1) return m_raw_value * (m_interpolation_point[0].coefficient * m_interpolation_point_size);

        // Value is within the range of the interpolation_point buffer
        if(m_interpolation_point[0].value < m_raw_value && m_raw_value < m_interpolation_point[m_interpolation_point_size-1].value)
        {
            for(uint_t i = 1; i<m_interpolation_point_size; ++i)
            {
                data_t x1 = m_interpolation_point[i-1].value;
                data_t x2 = m_interpolation_point[i].value;

                if(x1<=m_raw_value && m_raw_value<=x2)
                {
                    float y1 = m_interpolation_point[i-1].coefficient;
                    float y2 = m_interpolation_point[i].coefficient;

                    // If both compensation points coincide, then take the mean value. Else calculate a linear interpolation value
                    if(x2 == x1) return m_raw_value*((y1+y2)/2);
                    else return m_raw_value*(y1+(y2-y1)*((m_raw_value-x1)/(x2-x1)));
                }
            }

        }
        // Value is lower than the lowest value in the interpolation buffer
        else if(m_raw_value <= m_interpolation_point[0].value) return m_raw_value * m_interpolation_point[0].coefficient;
        // If the value is not within the range and is not lower, then it must be greate than the greates value in the interpolation buffer
        else return m_raw_value * m_interpolation_point[m_interpolation_point_size-1].coefficient;

        return m_raw_value;
    }

    template<class data_t, class uint_t>
    void Interpolation<data_t, uint_t>::in(const data_t &value)
    {
        m_raw_value = value;
    }

    template<class data_t, class uint_t>
    void Interpolation<data_t, uint_t>::reset(InterpolationPoint *interpolation_points, uint_t size)
    {
        m_interpolation_point = interpolation_points;
        m_raw_value = data_t();

        if(!interpolation_points) m_interpolation_point_size = 0;
        else  m_interpolation_point_size = size;
    }

    template<class data_t, class uint_t>
    void Interpolation<data_t, uint_t>::reset()
    {
        m_raw_value = data_t();
    }

    template<class data_t, class uint_t>
    void Interpolation<data_t, uint_t>::setPoint(uint_t index, data_t real_value, data_t measured_value)
    {
        if(index>(m_interpolation_point_size-1)) return;

        m_interpolation_point[index].value = measured_value;
        m_interpolation_point[index].coefficient = real_value/measured_value;
    }

}

#endif // INTERPOLATION_H
