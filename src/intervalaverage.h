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

#ifndef INTERVALAVERAGE_H
#define INTERVALAVERAGE_H

#include <type_traits>

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class IntervalAverage
    {
        public:
            IntervalAverage(uint_t interval);
            data_t out();
            void in(const data_t &value);
            void reset(uint_t interval);
            void reset();

        private:
            data_t m_sum;
            data_t m_avg;
            uint_t m_interval;
            uint_t m_count;

    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    IntervalAverage<data_t, uint_t>::IntervalAverage(uint_t interval):
        m_sum(data_t()),
        m_avg(data_t()),
        m_interval(interval),
        m_count(0)
    {
        static_assert (std::is_unsigned<uint_t>::value, "");
    }

    template <class data_t, class uint_t>
    data_t IntervalAverage<data_t, uint_t>::out()
    {
        return m_avg;
    }

    template<class data_t, class uint_t>
    void IntervalAverage<data_t, uint_t>::in(const data_t &value)
    {
        m_sum += value;
        ++m_count;
        if(m_count==m_interval)
        {
            m_avg = m_sum/m_interval;
            m_count = 0;
            m_sum = data_t();
        }
    }

    template <class data_t, class uint_t>
    void IntervalAverage<data_t, uint_t>::reset(uint_t interval)
    {
        m_sum = data_t();
        m_avg = data_t();
        m_interval = interval;
        m_count = 0;
    }

    template <class data_t, class uint_t>
    void IntervalAverage<data_t, uint_t>::reset()
    {
        m_sum = data_t();
        m_avg = data_t();
        m_count = 0;
    }
}

#endif // INTERVALAVERAGE_H
