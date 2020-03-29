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

#ifndef MOVINGMOSTFREQUENTOCCURANCE_H
#define MOVINGMOSTFREQUENTOCCURANCE_H

/*
 * DESCRIPTION
 * -----------
 * Select the most frequently occured element in the buffer.
 *
 * ALGORITHM
 * ---------
 *
 * PROS
 * ----
* 1. The returned value is a value from the buffer
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
 * uint_t - Type of unsigned integers used. This type should be chosen carefully based on the CPU/MCU for optimal performance.
 */

#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class MovingMostFrequentOccurance: protected buffer::Buffer<data_t, uint_t>
    {
        public:
            struct Occurance
            {
                    data_t value;
                    uint_t counter = 0;
            };

        public:
            MovingMostFrequentOccurance(data_t *buffer, Occurance *occurance_buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t &value);
            void reset(data_t *buffer, Occurance *occurance_buffer, uint_t buffer_size);
            void reset();

        private:
            Occurance *m_occurance_buffer;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingMostFrequentOccurance<data_t, uint_t>::MovingMostFrequentOccurance(data_t *buffer, Occurance *occurance_buffer, uint_t buffer_size):
        buffer::Buffer<data_t, uint_t>(buffer, buffer_size),
        m_occurance_buffer(occurance_buffer)
    {
    }

    template<class data_t, class uint_t>
    data_t MovingMostFrequentOccurance<data_t, uint_t>::out()
    {
        if(!m_occurance_buffer || !buffer::Buffer<data_t, uint_t>::valid()) return data_t();

        Occurance mfo = m_occurance_buffer[0];

        uint_t buffer_size = buffer::Buffer<data_t, uint_t>::count();
        for(uint_t i = 1; i < buffer_size; ++i)
        {
            if(m_occurance_buffer[i].counter > mfo.counter) mfo = m_occurance_buffer[i];
        }

        return mfo.value;
    }

    template<class data_t, class uint_t>
    void MovingMostFrequentOccurance<data_t, uint_t>::in(const data_t &value)
    {
        if(!buffer::Buffer<data_t, uint_t>::valid()) return;

        uint_t buffer_size= buffer::Buffer<data_t, uint_t>::size();

        // If buffer is full, take into account the poped out value
        if(buffer::Buffer<data_t, uint_t>::full())
        {
            const data_t last = buffer::Buffer<data_t, uint_t>::last();

            // Same value is inserted and poped out, so no action is required because the MFO is not changed
            if(value == last)
            {
                buffer::Buffer<data_t, uint_t>::pushFront(value);
                return;
            }

            // Decrease the poped value counter and clear the value it if the number of occurances is 0
            for(uint_t i = 0; i < buffer_size; ++i)
            {
                if(m_occurance_buffer[i].value == last)
                {
                    --m_occurance_buffer[i].counter;
                    if(m_occurance_buffer[i].counter == 0) m_occurance_buffer[i].value = data_t();
                    break;
                }
            }
        }

        buffer::Buffer<data_t, uint_t>::pushFront(value);

        uint_t index = 0;
        // Find the index of the pushed value or select an empty index
        for(uint_t i = 0; i < buffer_size; ++i)
        {
            if(m_occurance_buffer[i].value == value)
            {
                index = i;
                break;
            }
            if(m_occurance_buffer[i].counter == 0) index = i;
        }

        m_occurance_buffer[index].value = value;
        ++m_occurance_buffer[index].counter;
    }

    template<class data_t, class uint_t>
    void MovingMostFrequentOccurance<data_t, uint_t>::reset(data_t *buffer, Occurance *occurance_buffer, uint_t buffer_size)
    {
        m_occurance_buffer = occurance_buffer;
        buffer::Buffer<data_t, uint_t>::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingMostFrequentOccurance<data_t, uint_t>::reset()
    {
        buffer::Buffer<data_t, uint_t>::clear();
    }
}
#endif // MOVINGMOSTFREQUENTOCCURANCE_H
