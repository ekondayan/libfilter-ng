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
 * Track the number of occurrences of each value, then select the most
 * frequently occured. This filter works best for stable signal values.
 * For rapidly changing signals, it is not of much use.
 *
 * ALGORITHM
 * ---------
 *
 * PROS
 * ----
* 1. The returned value is a real measurement value from the buffer
* 2. Good for steady signal
* 3. Removes outliers
 *
 * CONS
 * ----
 * 1. Slow
 * 2. The output from a changing signal looks like a stair case
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

#ifndef MOVINGMOSTFREQUENTOCCURRENCE_H
#define MOVINGMOSTFREQUENTOCCURRENCE_H

#include <type_traits>
#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class MovingMostFrequentOccurrence: protected buffer::Buffer<data_t, uint_t>
    {
            using Buffer = buffer::Buffer<data_t, uint_t>;

        public:
            struct Occurrence
            {
                    data_t value;
                    uint_t counter = 0;
            };

        public:
            MovingMostFrequentOccurrence(data_t* buffer, Occurrence* occurrence_buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t& value);
            void reset(data_t* buffer, Occurrence* occurrence_buffer, uint_t buffer_size);
            void reset();

            using Buffer::valid;

        private:
            Occurrence* m_occurrence_buffer;
    };

    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingMostFrequentOccurrence<data_t, uint_t>::MovingMostFrequentOccurrence(data_t* buffer, Occurrence* occurrence_buffer, uint_t buffer_size):
        Buffer(buffer, buffer_size),
        m_occurrence_buffer(occurrence_buffer)
    {
        static_assert (std::is_unsigned_v<uint_t>, "Template type \"uint_t\" expected to be of unsigned numeric type");
    }

    template<class data_t, class uint_t>
    data_t MovingMostFrequentOccurrence<data_t, uint_t>::out()
    {
        if(!m_occurrence_buffer || !Buffer::valid())
            return data_t();

        Occurrence mfo = m_occurrence_buffer[0];

        uint_t buffer_size = Buffer::count();
        for(uint_t i = 1; i < buffer_size; ++i)
        {
            if(m_occurrence_buffer[i].counter > mfo.counter)
                mfo = m_occurrence_buffer[i];
        }

        return mfo.value;
    }

    template<class data_t, class uint_t>
    void MovingMostFrequentOccurrence<data_t, uint_t>::in(const data_t& value)
    {
        if(!m_occurrence_buffer || !Buffer::valid())
            return;

        uint_t buffer_size= Buffer::size();

        // If the buffer is full, take into account the poped out value
        if(Buffer::full())
        {
            const data_t last = Buffer::last();

            // Same value is inserted and poped out, so no action is required because the MFO is not changed
            if(value == last)
            {
                Buffer::pushFront(value);
                return;
            }

            // Decrease the poped value counter and clear the value if the number of occurrences is 0
            for(uint_t i = 0; i < buffer_size; ++i)
            {
                if(m_occurrence_buffer[i].value == last)
                {
                    --m_occurrence_buffer[i].counter;
                    if(m_occurrence_buffer[i].counter == 0)
                        m_occurrence_buffer[i].value = data_t();
                    break;
                }
            }
        }

        Buffer::pushFront(value);

        uint_t index = 0;
        // Find the index of the pushed value or select an empty index
        for(uint_t i = 0; i < buffer_size; ++i)
        {
            if(m_occurrence_buffer[i].value == value)
            {
                index = i;
                break;
            }
            if(m_occurrence_buffer[i].counter == 0)
                index = i;
        }

        m_occurrence_buffer[index].value = value;
        ++m_occurrence_buffer[index].counter;
    }

    template<class data_t, class uint_t>
    void MovingMostFrequentOccurrence<data_t, uint_t>::reset(data_t* buffer, Occurrence* occurrence_buffer, uint_t buffer_size)
    {
        m_occurrence_buffer = occurrence_buffer;
        Buffer::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingMostFrequentOccurrence<data_t, uint_t>::reset()
    {
        Buffer::clear();
    }
}
#endif // MOVINGMOSTFREQUENTOCCURRENCE_H
