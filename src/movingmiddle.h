#ifndef MOVINGMIDDLE_H
#define MOVINGMIDDLE_H

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
 * 1. The returned value is a value from the buffer
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
 * uint_t - Type of unsigned integers used. This type should be chosen carefully based on the CPU/MCU for optimal performance.
 */

#include "buffer.h"

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    class MovingMiddle: protected buffer::Buffer<data_t, uint_t>
    {
        public:
            MovingMiddle(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t &value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();

        private:
            data_t m_min;
            data_t m_max;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingMiddle<data_t, uint_t>::MovingMiddle(data_t *buffer, uint_t buffer_size):
        buffer::Buffer<data_t, uint_t>(buffer, buffer_size),
        m_min(data_t()),
        m_max(data_t())
    {
    }

    template<class data_t, class uint_t>
    data_t MovingMiddle<data_t, uint_t>::out()
    {
        // There should be at least two elements to calculate the middle element
        if(buffer::Buffer<data_t, uint_t>::count() < 2) return data_t();

        // Calculate the aritmetic middle
        const data_t middle = m_min + ((m_max - m_min) / 2.0F);

        // Clear the prevoius middle value and set it to the first element
        data_t element = buffer::Buffer<data_t, uint_t>::first();

        // Calculate the aritmetic distance of the element to the middle value
        data_t element_to_middle_dist = (middle > element?middle-element:element-middle);

        // Find the value with the minimum distance to the middle value
        for(uint_t i = 1; i<buffer::Buffer<data_t, uint_t>::count(); ++i)
        {
            data_t current = buffer::Buffer<data_t, uint_t>::at(i);
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
    void MovingMiddle<data_t, uint_t>::in(const data_t &value)
    {
        if(!buffer::Buffer<data_t, uint_t>::valid()) return;

        // When the buffer is full take into account the poped value
        if(buffer::Buffer<data_t, uint_t>::full())
        {
            const data_t last = buffer::Buffer<data_t, uint_t>::last();

            buffer::Buffer<data_t, uint_t>::pushFront(value);

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
                m_min = buffer::Buffer<data_t, uint_t>::first();
                m_max = m_min;
                uint_t buffer_count = buffer::Buffer<data_t, uint_t>::count();
                for(uint_t i = 1; i < buffer_count; ++i)
                {
                    data_t current = buffer::Buffer<data_t, uint_t>::at(i);
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
            buffer::Buffer<data_t, uint_t>::pushFront(value);

            // Set initial values for the min and max
            if(buffer::Buffer<data_t, uint_t>::count() == 1) m_min = m_max = value;
            else if(value < m_min) m_min = value;
            else if(value > m_max) m_max = value;
        }
    }

    template<class data_t, class uint_t>
    void MovingMiddle<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        m_min = data_t();
        m_max = data_t();
        buffer::Buffer<data_t, uint_t>::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingMiddle<data_t, uint_t>::reset()
    {
        m_min = data_t();
        m_max = data_t();
        buffer::Buffer<data_t, uint_t>::clear();
    }
}

#endif // MOVINGMIDDLE_H
