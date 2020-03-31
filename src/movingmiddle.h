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
 */

/*
 * DESCRIPTION
 * -----------
 * Select the closest element to the aritmetic middle between the minimum and the maximum
 * values in the buffer.
 *
 * ALGORITHM
 * ---------
 * 1. Find the minimum and maximum values in the buffer
 * 2. Calculate the aritmetic middle point between the min and max
 * 3. Set the middle element to be the first element. This is the initial condition
 * 4. Cycle trough all the elements and calculate the distance of every element to the middle
 * 5. If the distance of the current element is smaller than the last element,
 *    then set the element to be the current one
 *
 * PROS
 * ----
 * 1. The returned value is a real measurement value from the buffer
 * 2. Dampens outliers
 *
 * CONS
 * ----
 * 1. Slow
 * 2. Does not eliminate outliers as good as median filter
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

#ifndef MOVINGMIDDLE_H
#define MOVINGMIDDLE_H

#include <type_traits>
#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class MovingMiddle: protected buffer::Buffer<data_t, uint_t>
    {
            using Buffer = buffer::Buffer<data_t, uint_t>;

        public:
            MovingMiddle(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t& value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();

            using Buffer::valid;

        private:
            data_t m_min;
            data_t m_max;
    };

    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingMiddle<data_t, uint_t>::MovingMiddle(data_t *buffer, uint_t buffer_size):
        Buffer(buffer, buffer_size),
        m_min(data_t()),
        m_max(data_t())
    {
        static_assert (std::is_unsigned_v<uint_t>, "Template type \"uint_t\" expected to be of unsigned numeric type");
    }

    template<class data_t, class uint_t>
    data_t MovingMiddle<data_t, uint_t>::out()
    {
        // There should be at least two elements to calculate the middle element
        if(Buffer::count() < 2)
            return data_t();

        // Calculate the aritmetic middle
        const data_t middle = m_min + ((m_max - m_min) / 2.0F);

        // Clear the prevoius middle value and set it to the first element
        data_t element = Buffer::first();

        // Calculate the aritmetic distance of the element to the middle value
        data_t element_to_middle_dist = (middle > element?middle-element:element-middle);

        // Find the value with the minimum distance to the middle value
        for(uint_t i = 1; i<Buffer::count(); ++i)
        {
            data_t current = Buffer::at(i);
            data_t cur_to_middle_dist = (middle > current?middle-current:current-middle);

            // Save the element if the distance is closer than the previous one
            if(cur_to_middle_dist<element_to_middle_dist)
            {
                element = current;
                element_to_middle_dist = (middle > element?middle-element:element-middle);
            }
        }

        return element;
    }

    template<class data_t, class uint_t>
    void MovingMiddle<data_t, uint_t>::in(const data_t& value)
    {
        if(!Buffer::valid())
            return;

        // When the buffer is full take into account the poped value
        if(Buffer::full())
        {
            const data_t last = Buffer::last();

            Buffer::pushFront(value);

            // Same value is inserted and poped out, so no action is required because the middle is not changed
            if(value == last) return;
            // Minimum value poped out and new value less than min
            else if(last == m_min && value < m_min) m_min = value;
            // Maximum value poped out and new value greater than max
            else if(last == m_max && value > m_max) m_max = value;
            // Maximum or minimum value poped out but new value between min and max
            else if(last == m_min || last == m_max)
            {
                // Set initial value for the min and max
                m_min = Buffer::first();
                m_max = m_min;
                uint_t buffer_count = Buffer::count();
                for(uint_t i = 1; i < buffer_count; ++i)
                {
                    data_t current = Buffer::at(i);
                    if(current < m_min) m_min = current;
                    else if(current > m_max) m_max = current;
                }
            }
            // Maximum value is not poped but a greater one is put in
            else if(value > m_max) m_max = value;
            // Minimum value is not poped but a lesser one is put in
            else if(value < m_min) m_min = value;
        }
        // Buffer is not full, thus the min and max valuer are calculated using a simple compare operation
        else
        {
            Buffer::pushFront(value);

            // Set initial values for the min and max
            if(Buffer::count() == 1)
                m_min = m_max = value;
            else if(value < m_min)
                m_min = value;
            else if(value > m_max)
                m_max = value;
        }
    }

    template<class data_t, class uint_t>
    void MovingMiddle<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        m_min = data_t();
        m_max = data_t();
        Buffer::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingMiddle<data_t, uint_t>::reset()
    {
        m_min = data_t();
        m_max = data_t();
        Buffer::clear();
    }
}

#endif // MOVINGMIDDLE_H
