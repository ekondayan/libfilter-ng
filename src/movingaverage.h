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
 * written in C++17 with efficiency in mind
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
 * uint_t - Type of unsigned integers used troughout the class.
 *          This type should be chosen carefully based on the CPU/MCU for
 *          optimal performance. A default type of 16-bit unsigned int is
 *          sufficient for most cases.
 */

#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <type_traits>
#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class MovingAverage: protected buffer::Buffer<data_t, uint_t>
    {
            using Buffer = buffer::Buffer<data_t, uint_t>;

        public:
            MovingAverage(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t& value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();

            using Buffer::valid;

        private:
            data_t m_sum;
    };

    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingAverage<data_t, uint_t>::MovingAverage(data_t *buffer, uint_t buffer_size):
        Buffer(buffer, buffer_size),
        m_sum(data_t())
    {
        static_assert (std::is_unsigned_v<uint_t>, "Template type \"uint_t\" expected to be of unsigned numeric type");
    }

    template<class data_t, class uint_t>
    data_t MovingAverage<data_t, uint_t>::out()
    {
        return m_sum/Buffer::count();
    }

    template<class data_t, class uint_t>
    void MovingAverage<data_t, uint_t>::in(const data_t& value)
    {
        if(!Buffer::valid()) return;

        if(Buffer::full()) m_sum -= Buffer::last();
        m_sum += value;

        Buffer::pushFront(value);
    }

    template<class data_t, class uint_t>
    void MovingAverage<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        m_sum = data_t();
        Buffer::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingAverage<data_t, uint_t>::reset()
    {
        m_sum = data_t();
        Buffer::clear();
    }
}

#endif // MOVINGAVERAGE_H
