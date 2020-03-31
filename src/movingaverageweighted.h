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
 * Weighted moving averages assign a heavier weighting to more current data
 * points since they are more relevant than data points in the distant past.
 * The sum of the weighting should add up to 1 (or 100 percent).
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

#ifndef MOVINGAVERAGEWEIGHTED_H
#define MOVINGAVERAGEWEIGHTED_H

#include <type_traits>
#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class MovingWeightedAverage: protected buffer::Buffer<data_t, uint_t>
    {
            using Buffer = buffer::Buffer<data_t, uint_t>;

        public:
            MovingWeightedAverage(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t& value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();

            using Buffer::valid;

        private:
            uint_t m_triangular_number;
    };

    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingWeightedAverage<data_t, uint_t>::MovingWeightedAverage(data_t *buffer, uint_t buffer_size):
        Buffer(buffer, buffer_size),
        m_triangular_number(((buffer_size-1)*buffer_size)/2)
    {
        static_assert (std::is_unsigned_v<uint_t>, "Template type \"uint_t\" expected to be of unsigned numeric type");
    }

    template<class data_t, class uint_t>
    data_t MovingWeightedAverage<data_t, uint_t>::out()
    {
        data_t wma;
        uint_t buffer_count = Buffer::count();
        for(uint_t i = 0; i<buffer_count; ++i) wma += Buffer::at(i)*(float(buffer_count-i)/m_triangular_number);

        return wma;
    }

    template<class data_t, class uint_t>
    void MovingWeightedAverage<data_t, uint_t>::in(const data_t& value)
    {
        if(!Buffer::valid()) return;

        if(!Buffer::full())
        {
            Buffer::pushFront(value);
            uint_t buffer_count = Buffer::count();
            m_triangular_number = (buffer_count*(buffer_count+1))/2;
        }
        else Buffer::pushFront(value);
    }

    template<class data_t, class uint_t>
    void MovingWeightedAverage<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        if(!buffer) m_triangular_number = 0;
        else m_triangular_number = ((buffer_size-1)*buffer_size)/2;

        Buffer::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingWeightedAverage<data_t, uint_t>::reset()
    {
        Buffer::clear();
    }
}
#endif // MOVINGAVERAGEWEIGHTED_H
