#ifndef HIPASS_H
#define HIPASS_H

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
 * IIR
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
    class HiPass
    {
        public:
            HiPass(float alpha, uint_t offset = 0);
            data_t out();
            void in(const data_t &value);
            void reset(float alpha, uint_t offset);
            void reset();

        private:
            data_t m_hipass;
            float  m_alpha;
            uint_t m_first_value_offset;
            data_t m_value_last;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template <class data_t, class uint_t>
    HiPass<data_t, uint_t>::HiPass(float alpha, uint_t offset):
        m_hipass(data_t()),
        m_alpha(alpha),
        m_first_value_offset(offset + 2),
        m_value_last(data_t())
    {

    }

    template <class data_t, class uint_t>
    data_t HiPass<data_t, uint_t>::out()
    {
        return m_hipass;
    }

    template <class data_t, class uint_t>
    void HiPass<data_t, uint_t>::in(const data_t &value)
    {
        if(m_first_value_offset == 0)
        {
            m_hipass = m_alpha * (m_hipass + value - m_value_last); //Refactored from [m_alpha * m_hipass + m_alpha * (value - m_value_last)]
            m_value_last = value;
        }
        else
        {
            --m_first_value_offset;
            m_value_last = m_hipass;
            m_hipass = value;
        }
    }

    template <class data_t, class uint_t>
    void HiPass<data_t, uint_t>::reset(float alpha, uint_t offset)
    {
        m_hipass = data_t();
        m_alpha = alpha;
        m_first_value_offset = offset + 2;
        m_value_last = data_t();
    }

    template <class data_t, class uint_t>
    void HiPass<data_t, uint_t>::reset()
    {
        m_hipass = data_t();
        m_value_last = data_t();
    }
}

#endif // HIPASS_H
