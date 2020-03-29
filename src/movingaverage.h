#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

/*
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

#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    class MovingAverage: protected buffer::Buffer<data_t, uint_t>
    {
        public:
            MovingAverage(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t &value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();

        private:
            data_t m_sum;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingAverage<data_t, uint_t>::MovingAverage(data_t *buffer, uint_t buffer_size):
        buffer::Buffer<data_t, uint_t>(buffer, buffer_size),
        m_sum(data_t())
    {
    }

    template<class data_t, class uint_t>
    data_t MovingAverage<data_t, uint_t>::out()
    {
        return m_sum/buffer::Buffer<data_t, uint_t>::count();
    }

    template<class data_t, class uint_t>
    void MovingAverage<data_t, uint_t>::in(const data_t &value)
    {
        if(!buffer::Buffer<data_t, uint_t>::valid()) return;

        if(buffer::Buffer<data_t, uint_t>::full()) m_sum -= buffer::Buffer<data_t, uint_t>::last();
        m_sum += value;

        buffer::Buffer<data_t, uint_t>::pushFront(value);
    }

    template<class data_t, class uint_t>
    void MovingAverage<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        m_sum = data_t();
        buffer::Buffer<data_t, uint_t>::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingAverage<data_t, uint_t>::reset()
    {
        m_sum = data_t();
        buffer::Buffer<data_t, uint_t>::clear();
    }
}

#endif // MOVINGAVERAGE_H
