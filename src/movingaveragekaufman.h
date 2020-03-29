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
 * Kaufman Adaptive Moving Average (KAMA) was created by Perry Kaufman and first presented in his book Smarter Trading (1995).
 * This moving average offered a significant advantage over previous attempts at ‘intelligent’ averages because it allowed the
 * user greater control. It is very similar to the Exponential Moving Average except the 'α'  parameter: EMA = EMA(1) + α * (value – EMA(1))
 *
 * The Variable Moving Average – VMA (1992) for instance offered no upper or lower limit to its
 * smoothing period. The AMA on the other hand allowed the user to define the range across which they desired the smoothing to be spread.
 * It follows the same theory as the VMA in that there will be different amounts of noise and therefore a different moving average speed
 * will be required to achieve the most profitable results. In a strongly trending market for instance,
 * the noise levels are low and a faster moving average should produce the best results.  Conversely in a crab or sideways market the noise levels
 * are very high and a slower average is likely to be better suited.
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
 * IIR
 *
 * DATA TYPES
 * ----------
 * data_t - Type of the data, the filter will work with
 * uint_t - Type of unsigned integers used. This type should be chosen carefully based on the CPU/MCU for optimal performance.
 */

#ifndef MOVINGAVERAGEKAUFMAN_H
#define MOVINGAVERAGEKAUFMAN_H

#include <type_traits>
#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t = unsigned short int>
    class MovingAverageKaufman: protected buffer::Buffer<data_t, uint_t>
    {
        public:
            MovingAverageKaufman(data_t *buffer, uint_t buffer_size, uint_t er_periods, uint_t slow_periods, uint_t fast_periods);
            data_t out();
            void in(const data_t &value);
            void reset(data_t *buffer, uint_t buffer_size, uint_t er_periods, uint_t slow_periods, uint_t fast_periods);
            void reset();

            using buffer::Buffer<data_t, uint_t>::valid;

        private:
            data_t abs(const data_t &value);

        private:
            uint_t m_er_periods;
            uint_t m_slow_periods;
            uint_t m_fast_periods;
            data_t m_kama;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingAverageKaufman<data_t, uint_t>::MovingAverageKaufman(data_t *buffer, uint_t buffer_size, uint_t er_periods, uint_t slow_periods, uint_t fast_periods):
        buffer::Buffer<data_t, uint_t>(buffer, buffer_size),
        m_er_periods(er_periods),
        m_slow_periods(slow_periods),
        m_fast_periods(fast_periods),
        m_kama(data_t())
    {
        static_assert (std::is_unsigned_v<uint_t>, "Template type \"uint_t\" expected to be of unsigned numeric type");
    }

    template<class data_t, class uint_t>
    data_t MovingAverageKaufman<data_t, uint_t>::out()
    {
        // TODO: Not correct
        if(!buffer::Buffer<data_t, uint_t>::full()) return data_t();

        data_t change = abs(buffer::Buffer<data_t, uint_t>::first() - buffer::Buffer<data_t, uint_t>::at(m_er_periods));

        // TODO: The for loop can be eliminated if this calculation is moved to the in() method
        data_t volatility = data_t();
        for(uint_t i = 0; i < m_er_periods; ++i) volatility += abs(buffer::Buffer<data_t, uint_t>::at(i) - buffer::Buffer<data_t, uint_t>::at(i+1));

        data_t er = (volatility == 0)? 0 : (change / volatility);

        data_t sc = er*( 2.0F/(m_fast_periods + 1) - 2.0F/(m_slow_periods + 1) ) + 2.0F/(m_slow_periods + 1);
        sc *= sc;

        m_kama = sc * (buffer::Buffer<data_t, uint_t>::first() - m_kama) + m_kama;

        return m_kama;
    }

    template<class data_t, class uint_t>
    void MovingAverageKaufman<data_t, uint_t>::in(const data_t &value)
    {
        buffer::Buffer<data_t, uint_t>::pushFront(value);
    }

    template<class data_t, class uint_t>
    void MovingAverageKaufman<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size, uint_t er_periods, uint_t slow_periods, uint_t fast_periods)
    {
        m_er_periods = er_periods;
        m_slow_periods = slow_periods;
        m_fast_periods = fast_periods;

        buffer::Buffer<data_t, uint_t>::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingAverageKaufman<data_t, uint_t>::reset()
    {
        buffer::Buffer<data_t, uint_t>::clear();
    }

    template<class data_t, class uint_t>
    data_t MovingAverageKaufman<data_t, uint_t>::abs(const data_t &value)
    {
        if(value<0) return -value;
        return value;
    }


}

#endif // MOVINGAVERAGEKAUFMAN_H
