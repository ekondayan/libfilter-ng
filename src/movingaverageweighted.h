#ifndef MOVINGAVERAGEWEIGHTED_H
#define MOVINGAVERAGEWEIGHTED_H

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
    class MovingWeightedAverage: protected buffer::Buffer<data_t, uint_t>
    {
        public:
            MovingWeightedAverage(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t &value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();

        private:
            uint_t m_triangular_number;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingWeightedAverage<data_t, uint_t>::MovingWeightedAverage(data_t *buffer, uint_t buffer_size):
        buffer::Buffer<data_t, uint_t>(buffer, buffer_size),
        m_triangular_number(((buffer_size-1)*buffer_size)/2)
    {
    }

    template<class data_t, class uint_t>
    data_t MovingWeightedAverage<data_t, uint_t>::out()
    {
        data_t wma;
        uint_t buffer_count = buffer::Buffer<data_t, uint_t>::count();
        for(uint_t i = 0; i<buffer_count; ++i) wma += buffer::Buffer<data_t, uint_t>::at(i)*(float(buffer_count-i)/m_triangular_number);

        return wma;
    }

    template<class data_t, class uint_t>
    void MovingWeightedAverage<data_t, uint_t>::in(const data_t &value)
    {
        if(!buffer::Buffer<data_t, uint_t>::valid()) return;

        if(!buffer::Buffer<data_t, uint_t>::full())
        {
            buffer::Buffer<data_t, uint_t>::pushFront(value);
            uint_t buffer_count = buffer::Buffer<data_t, uint_t>::count();
            m_triangular_number = (buffer_count*(buffer_count+1))/2;
        }
        else buffer::Buffer<data_t, uint_t>::pushFront(value);
    }

    template<class data_t, class uint_t>
    void MovingWeightedAverage<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        if(!buffer) m_triangular_number = 0;
        else m_triangular_number = ((buffer_size-1)*buffer_size)/2;

        buffer::Buffer<data_t, uint_t>::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingWeightedAverage<data_t, uint_t>::reset()
    {
        buffer::Buffer<data_t, uint_t>::clear();
    }
}
#endif // MOVINGAVERAGEWEIGHTED_H
