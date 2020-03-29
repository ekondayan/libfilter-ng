#ifndef MOVINGAVERAGEEXP_H
#define MOVINGAVERAGEEXP_H

/*
 * DESCRIPTION
 * -----------
 * The Exponential Moving Average (EMA) is a very popular method for smoothing data
 * in an attempt to eliminate noise. Unlike the Simple Moving Average (SMA) that applies
 * equal weight to all data, the EMA applies more weight to the recent data so that it
 * reacts faster to sudden changes, thus reduce lag. Moving averages lag because they
 * are based on past values. The weighting applied to the most recent value depends on
 * the number of periods in the moving average.
 *
 * ALGORITHM
 * ---------
 * 1. Set the initial EMA value, by skipping 'n' values
 * 2. Calculate the weighting multiplier α = 2 / (periods + 1)
 * 3. Calculate EMA = EMA(1) + α * (new_value – EMA(1))
 *
 * PROS
 * ----
 * 1. Fast calculation times
 * 2. Change faster and is good at showing recent value swings
 *
 * CONS
 * ----
 * 1. Lags behing the real value
 *
 * TYPE
 * ----
 * IIR
 *
 * DATA TYPES
 * ----------
 * data_t - Type of the value
 * uint_t - Type of unsigned integers used. This type should be chosen carefully based on the CPU/MCU for optimal performance.
 */

namespace filter
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    class ExpMovingAverage
    {
        public:
            ExpMovingAverage(uint_t periods, uint_t first_value_offset = 0);
            data_t out();
            void in(const data_t &value);
            void reset(uint_t periods, uint_t first_value_offset = 0);
            void reset();

        private:
            data_t m_ema;
            float  m_alpha;
            uint_t m_first_value_offset;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    ExpMovingAverage<data_t, uint_t>::ExpMovingAverage(uint_t periods, uint_t first_value_offset):
        m_ema(data_t()),
        m_alpha(2.0F / (periods + 1)),
        m_first_value_offset(++first_value_offset)
    {

    }

    template<class data_t, class uint_t>
    data_t ExpMovingAverage<data_t, uint_t>::out()
    {
        return m_ema;
    }

    template<class data_t, class uint_t>
    void ExpMovingAverage<data_t, uint_t>::in(const data_t &value)
    {
        if(m_first_value_offset == 0) m_ema = m_alpha * (value - m_ema) + m_ema;
        else
        {
            --m_first_value_offset;
            m_ema = value;
        }
    }

    template<class data_t, class uint_t>
    void ExpMovingAverage<data_t, uint_t>::reset(uint_t periods, uint_t first_value_offset)
    {
        m_ema = data_t();
        m_alpha = 2.0F / (periods + 1);
        m_first_value_offset = ++first_value_offset;
    }

    template<class data_t, class uint_t>
    void ExpMovingAverage<data_t, uint_t>::reset()
    {
        m_ema = data_t();
    }
}

#endif // MOVINGAVERAGEEXP_H
