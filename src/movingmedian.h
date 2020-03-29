#ifndef MOVINGMEDIAN_H
#define MOVINGMEDIAN_H

//#include <algorithm>

#include "buffer.h"

/*
 * DESCRIPTION
 * -----------
 * Get the median value in the buffer.
 *
 * ALGORITHM
 * ---------
 * 1. It does not make sense to search for the median if element count is less than 3
 * 2. Calculate the index of the median
 * 3. For every element loop trough all the elements. This require two loops: outer and inner.
 *    In the inner loop count those that are less and those that are equal to the current.
 *    This will give us the exact position of the element/s in the buffer.
 * 4. In the outer loop test if the median index is between the count of the lesser elements and
 *    the count of lesser elements plus the count of the same element. If this is the case then
 *    this is the median and break out of the loop
 * 5. To speed up the algorithm even further, in the outer loop if the element is on the left side
 *    or on the right side of the median skip lesser and greater elements
 *
 * PROS
 * ----
 * 1. The returned value is a value from the buffer
 * 2. Remove outliers
 *
 * CONS
 * ----
 * 1. Slow
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

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    class MovingMedian: protected buffer::Buffer<data_t, uint_t>
    {
        public:
            MovingMedian(data_t *buffer, uint_t buffer_size);
            data_t out();
            void in(const data_t &value);
            void reset(data_t *buffer, uint_t buffer_size);
            void reset();
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    MovingMedian<data_t, uint_t>::MovingMedian(data_t *buffer, uint_t buffer_size):
        buffer::Buffer<data_t, uint_t>(buffer, buffer_size)
    {

    }

    template<class data_t, class uint_t>
    data_t MovingMedian<data_t, uint_t>::out()
    {
        uint_t buffer_count = buffer::Buffer<data_t, uint_t>::count();

        if(buffer_count < 3) return data_t();

        // 1. Calculate the index of the median
        uint_t middle_index = buffer_count/2;
        data_t median_element;
        data_t skip_greater_than;
        data_t skip_lesser_than;
        bool skip_greater_than_found = false;
        bool skip_lesser_than_found = false;

        // 2. Loop trough all the elements
        for(uint_t i = 0; i < buffer_count; ++i)
        {
            data_t cur_element = buffer::Buffer<data_t, uint_t>::at(i);

            // 5. To speed up the algirithm, if the element is on the right side of the median, skip all greater elements
            if(skip_greater_than_found && cur_element >= skip_greater_than) continue;
            // 5. To speed up the algirithm, if the element is on the left side of the median, skip all lesser elements
            if(skip_lesser_than_found && cur_element <= skip_lesser_than) continue;

            uint_t left_index= 0;
            uint_t right_index = 0;

            /* 3. Count the smaller elements and those that are equal. That way we find the
             *    first and the last index. Since the inner loop does not skip if the current element pass by itself,
             *    it is counted twice and the right index can be calculated by subtracting 1
             */
            for(uint_t j = 0; j < buffer_count; ++j)
            {
                data_t cmp_element = buffer::Buffer<data_t, uint_t>::at(j);

                if(cmp_element < cur_element)
                {
                    ++left_index;
                    ++right_index;
                }
                /* We have to take into account that the inner loop does not skip an itteration
                 * when it pass by itself, thus the right index is allways +1. When we test
                 * at step (4.) we must take this into account and use the < sign and not <=
                 */
                else if(cmp_element == cur_element) ++right_index;
            }

            // 4. If this is the median break out of the loop and return it
            if(left_index <= middle_index && middle_index < right_index)
            {
                median_element = cur_element;
                break;
            }
            // 5. If the element is on the right side of the median, skip all others greater than it
            else if(middle_index < left_index)
            {
                if(skip_greater_than_found) skip_greater_than = cur_element;
                else
                {
                    skip_greater_than_found = true;
                    skip_greater_than = cur_element;
                }
            }
            // 5. If the element is on the left side of the median, skip all others lesser than it
            else if(middle_index >= right_index)
            {
                if(skip_lesser_than_found) skip_lesser_than = cur_element;
                else
                {
                    skip_lesser_than_found = true;
                    skip_lesser_than = cur_element;
                }
            }
        }

        return median_element;
    }

    template<class data_t, class uint_t>
    void MovingMedian<data_t, uint_t>::in(const data_t &value)
    {
        buffer::Buffer<data_t, uint_t>::pushFront(value);
    }

    template<class data_t, class uint_t>
    void MovingMedian<data_t, uint_t>::reset(data_t *buffer, uint_t buffer_size)
    {
        buffer::Buffer<data_t, uint_t>::init(buffer, buffer_size);
    }

    template<class data_t, class uint_t>
    void MovingMedian<data_t, uint_t>::reset()
    {
        buffer::Buffer<data_t, uint_t>::clear();
    }
}

#endif // MOVINGMEDIAN_H