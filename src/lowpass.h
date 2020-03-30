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
 */

/*
 * DESCRIPTION
 * -----------
 * Attenuates the high frequency components of a spectrum while ‘passing’
 * the low frequencies within a specified range.
 *
 * ALGORITHM
 * ---------
 * y[i] := α * x[i] + (1-α) * y[i-1]
 *
 * PROS
 * ----
 *
 * CONS
 * ----
 *
 * DATA TYPES
 * ----------
 * data_t - Type of the data, the filter will work with
 * uint_t - Type of unsigned integers used troughout the class.
 *          This type should be chosen carefully based on the CPU/MCU for
 *          optimal performance. A default type of 16-bit unsigned int is
 *          sufficient for most cases.
 */

#ifndef LOWPASS_H
#define LOWPASS_H

#include <type_traits>

namespace filter
{
    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class LowPass
    {
        public:
            LowPass(float alpha, uint_t first_value_offset = 0);
            data_t out();
            void in(const data_t& value);
            void reset(float alpha, uint_t first_value_offset);
            void reset();

        private:
            data_t m_lowpass;
            float  m_alpha;
            uint_t m_first_value_offset;
    };

    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    LowPass<data_t, uint_t>::LowPass(float alpha, uint_t first_value_offset):
        m_lowpass(data_t()),
        m_alpha(alpha),
        m_first_value_offset(++first_value_offset)
    {
        static_assert (std::is_unsigned_v<uint_t>, "Template type \"uint_t\" expected to be of unsigned numeric type");
    }

    template <class data_t, class uint_t>
    data_t LowPass<data_t, uint_t>::out()
    {
        return m_lowpass;
    }

    template <class data_t, class uint_t>
    void LowPass<data_t, uint_t>::in(const data_t& value)
    {
        if(m_first_value_offset == 0) m_lowpass = m_lowpass + m_alpha * (value - m_lowpass); //Refactored from [m_alpha * value + (1.0F - m_alpha) * m_lowpass]
        else
        {
            --m_first_value_offset;
            m_lowpass = value;
        }
    }

    template <class data_t, class uint_t>
    void LowPass<data_t, uint_t>::reset(float alpha, uint_t first_value_offset)
    {
        m_lowpass = data_t();
        m_alpha = alpha;
        m_first_value_offset = ++first_value_offset;
    }

    template <class data_t, class uint_t>
    void LowPass<data_t, uint_t>::reset()
    {
        m_lowpass = data_t();
    }
}

#endif // LOWPASS_H
