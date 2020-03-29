#ifndef LOWPASS_H
#define LOWPASS_H

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
    class LowPass
    {
        public:
            LowPass(float alpha, uint_t first_value_offset = 0);
            data_t out();
            void in(const data_t &value);
            void reset(float alpha, uint_t first_value_offset);
            void reset();

        private:
            data_t m_lowpass;
            float  m_alpha;
            uint_t m_first_value_offset;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    LowPass<data_t, uint_t>::LowPass(float alpha, uint_t first_value_offset):
        m_lowpass(data_t()),
        m_alpha(alpha),
        m_first_value_offset(++first_value_offset)
    {

    }

    template <class data_t, class uint_t>
    data_t LowPass<data_t, uint_t>::out()
    {
        return m_lowpass;
    }

    template <class data_t, class uint_t>
    void LowPass<data_t, uint_t>::in(const data_t &value)
    {
        if(m_first_value_offset == 0) m_lowpass = m_lowpass + m_alpha * (value - m_lowpass); //Refactored from [m_alpha * value + (1.0F - m_alpha) * m_lowpass]
        else
        {
            --m_first_value_offset;
            m_lowpass = value;
        }
    }

    template <class data_t, class uint_t>
    void LowPass<data_t, uint_t>::reset(float alpha, uint_t first_value_offset)
    {
        m_lowpass = data_t();
        m_alpha = alpha;
        m_first_value_offset = ++first_value_offset;
    }

    template <class data_t, class uint_t>
    void LowPass<data_t, uint_t>::reset()
    {
        m_lowpass = data_t();
    }
}

#endif // LOWPASS_H
