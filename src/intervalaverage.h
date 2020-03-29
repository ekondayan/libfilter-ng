#ifndef INTERVALAVERAGE_H
#define INTERVALAVERAGE_H

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

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    class IntervalAverage
    {
        public:
            IntervalAverage(uint_t interval);
            data_t out();
            void in(const data_t &value);
            void reset(uint_t interval);
            void reset();

        private:
            data_t m_sum;
            data_t m_avg;
            uint_t m_interval;
            uint_t m_count;

    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    IntervalAverage<data_t, uint_t>::IntervalAverage(uint_t interval):
        m_sum(data_t()),
        m_avg(data_t()),
        m_interval(interval),
        m_count(0)
    {

    }

    template <class data_t, class uint_t>
    data_t IntervalAverage<data_t, uint_t>::out()
    {
        return m_avg;
    }

    template<class data_t, class uint_t>
    void IntervalAverage<data_t, uint_t>::in(const data_t &value)
    {
        m_sum += value;
        ++m_count;
        if(m_count==m_interval)
        {
            m_avg = m_sum/m_interval;
            m_count = 0;
            m_sum = data_t();
        }
    }

    template <class data_t, class uint_t>
    void IntervalAverage<data_t, uint_t>::reset(uint_t interval)
    {
        m_sum = data_t();
        m_avg = data_t();
        m_interval = interval;
        m_count = 0;
    }

    template <class data_t, class uint_t>
    void IntervalAverage<data_t, uint_t>::reset()
    {
        m_sum = data_t();
        m_avg = data_t();
        m_count = 0;
    }
}

#endif // INTERVALAVERAGE_H
