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
 */

#ifndef BUFFER_H
#define BUFFER_H

/*
 * DESCRIPTION
 * -----------
 * Circular buffer optimized for speed. Pushing, poping and rotating use cheap
 * aritmetic operations, no modulus operator or conditional branching. For this
 * to be achieved the buffer size must be a power of two(2, 4, 8, 16, 32, ...).
 * Since this is circular buffer the real size is allways: the set value minus one. One element is
 * wasted for the head pointer. For example if you set the buffer size to 16 elements,
 * then the real size if 15 elements.
 * The buffer algorithms are self contained and do not use external dependencies. This
 * makes is suitable for embedded systems and MCUs.
 *
 * PROS
 * ----
 * 1. No expensive modulo operation to calculate the head and tail
 * 2. Optimized for MCU
 * 3. Simple API
 * 4. Template class. Can work with any data type
 * 5. No external dependencies. All the functions are self contined
 *
 * CONS
 * ----
 * 1. The size of the buffer passed to the constructor must have size a power of 2
 * 2. The real size is allways -1 bucause of the head pointer
 *
 * DATA TYPES
 * ----------
 * data_t - Type of the data the buffer will work with
 * uint_t - Type of unsigned integers used troughout the class.
 *          This type should be chosen carefully based on the CPU/MCU for optimal performance.
 */

namespace buffer
{
    /***********************************************************************/
    /***************************** Definition ******************************/
    /***********************************************************************/

    template <class data_t, class uint_t =  = unsigned short int> class Buffer
    {
        public:
            Buffer();
            Buffer(data_t *buffer, uint_t size, bool safe_erase = false);
            ~Buffer();

            inline Buffer &init(data_t *buffer = nullptr, uint_t size = 0, bool safe_erase = false);
            inline Buffer &pushFront(const data_t &value);
            inline Buffer &pushBack(const data_t &value);
            inline Buffer &popFront(data_t *value = nullptr);
            inline Buffer &popBack(data_t *value = nullptr);
            inline Buffer &clear();
            inline Buffer *getPtr();
            inline bool full();
            inline bool empty();
            inline bool valid();
            inline uint_t size();
            inline uint_t count();
            inline data_t first();
            inline data_t last();
            inline void erase();
            inline data_t at(uint_t index);
            inline Buffer &rotateForeward();
            inline Buffer &rotateBackward();
            inline void copyToArray(data_t *array, uint_t start = 0, uint_t count = 0);
            inline data_t &operator[](uint_t index);
            inline Buffer &operator<<(const data_t &value);
            inline Buffer &operator>>(data_t &value);

        private:
            uint_t m_buffer_mask;
            uint_t m_buffer_tail;
            uint_t m_buffer_head;
            uint_t m_buffer_count;
            data_t *m_buffer;
            bool   m_safe_erase;
    };

    /***********************************************************************/
    /***************************** Declaration *****************************/
    /***********************************************************************/

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t>::Buffer():
        Buffer(nullptr, 0)
    {
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t>::Buffer(data_t *buffer, uint_t size, bool safe_erase):
        m_buffer_mask(buffer?size-1:0),
        m_buffer_tail(0),
        m_buffer_head(0),
        m_buffer_count(0),
        m_buffer(buffer),
        m_safe_erase(safe_erase)
    {
        if(m_safe_erase) erase();
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t>::~Buffer()
    {
        if(m_safe_erase) erase();
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> *Buffer<data_t, uint_t>::getPtr()
    {
        return m_buffer;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::init(data_t *buffer, uint_t size, bool safe_erase)
    {
        // Reset the buffer indexes
        m_buffer_tail = 0;
        m_buffer_head = 0;
        m_buffer_count = 0;
        m_safe_erase = safe_erase;

        // If it is the same buffer, erase it if necessary
        if(buffer != m_buffer)
        {
            // Do not use a buffer and clear the member variables
            if(buffer == nullptr)
            {
                m_buffer_mask = 0;
                m_buffer = nullptr;
            }
            else
            {
                m_buffer_mask = size - 1;
                m_buffer = buffer;
            }
        }

        if(m_safe_erase) erase();

        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::pushFront(const data_t &value)
    {
        if(m_buffer == nullptr) return *this;

        m_buffer[m_buffer_head] = value;
        (++m_buffer_head) &= m_buffer_mask;
        if(m_buffer_head==m_buffer_tail) ++m_buffer_tail &= m_buffer_mask;
        else ++m_buffer_count;

        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::pushBack(const data_t &value)
    {
        if(m_buffer == nullptr) return *this;

        (--m_buffer_tail) &= m_buffer_mask;
        m_buffer[m_buffer_tail] = value;
        if(m_buffer_head==m_buffer_tail) (--m_buffer_head) &= m_buffer_mask;
        else ++m_buffer_count;

        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::popFront(data_t *value)
    {
        if(m_buffer == nullptr || m_buffer_tail == m_buffer_head) return *this;

        (--m_buffer_head) &= m_buffer_mask;
        if(value) *value = m_buffer[m_buffer_head];
        --m_buffer_count;

        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::popBack(data_t *value)
    {
        if(m_buffer == nullptr || m_buffer_tail == m_buffer_head) return *this;

        if(value) *value = m_buffer[m_buffer_tail];
        (++m_buffer_tail) &= m_buffer_mask;
        --m_buffer_count;

        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::clear()
    {
        m_buffer_tail = 0;
        m_buffer_head = 0;
        m_buffer_count = 0;

        if(m_safe_erase) erase();

        return *this;
    }

    template<class data_t, class uint_t>
    bool Buffer<data_t, uint_t>::full()
    {
        return m_buffer_count==m_buffer_mask;
    }

    template<class data_t, class uint_t>
    bool Buffer<data_t, uint_t>::empty()
    {
        return m_buffer_tail == m_buffer_head;
    }

    template<class data_t, class uint_t>
    bool Buffer<data_t, uint_t>::valid()
    {
        return m_buffer != nullptr;
    }

    template<class data_t, class uint_t>
    uint_t Buffer<data_t, uint_t>::size()
    {
        return m_buffer_mask==0?0:m_buffer_mask;
    }

    template<class data_t, class uint_t>
    uint_t Buffer<data_t, uint_t>::count()
    {
        return m_buffer_count;
    }

    template<class data_t, class uint_t>
    data_t Buffer<data_t, uint_t>::first()
    {
        if(m_buffer == nullptr || m_buffer_head == m_buffer_tail) return data_t();
        return m_buffer[(m_buffer_head-1) & m_buffer_mask];
    }

    template<class data_t, class uint_t>
    data_t Buffer<data_t, uint_t>::last()
    {
        if(m_buffer == nullptr || m_buffer_head == m_buffer_tail) return data_t();
        return m_buffer[m_buffer_tail];
    }

    template<class data_t, class uint_t>
    void Buffer<data_t, uint_t>::erase()
    {
        if(m_buffer == nullptr) return;

        for(uint_t i = 0; i<m_buffer_mask+1; ++i) m_buffer[i] = data_t();
    }

    template<class data_t, class uint_t>
    data_t Buffer<data_t, uint_t>::at(uint_t index)
    {
        if(m_buffer == nullptr || index > (m_buffer_count - 1)) return data_t();
        return m_buffer[(m_buffer_head - 1 - index) & m_buffer_mask];
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::rotateForeward()
    {
        // Should rotate only full buffer
        if(m_buffer_count!=m_buffer_mask) return *this;

        // Swap tail and head elements. This is necessary because head is pointing to an invalid element
        data_t tmp_swap = m_buffer[m_buffer_head];
        m_buffer[m_buffer_head] = m_buffer[m_buffer_tail];
        m_buffer[m_buffer_tail] = tmp_swap;

        (++m_buffer_head) &= m_buffer_mask;
        (++m_buffer_tail) &= m_buffer_mask;

        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::rotateBackward()
    {
        // Should rotate only full buffer
        if(m_buffer_count!=m_buffer_mask) return *this;

        // Swap head and the first elements. This is necessary because head is pointing to an invalid element
        data_t tmp_swap = m_buffer[(m_buffer_head - 1) & m_buffer_mask];
        m_buffer[(m_buffer_head - 1) & m_buffer_mask] = m_buffer[m_buffer_head];
        m_buffer[m_buffer_head] = tmp_swap;

        (--m_buffer_head) &= m_buffer_mask;
        (--m_buffer_tail) &= m_buffer_mask;

        return *this;
    }

    // !!! IMPORTANT - OPTIMIZE FOR SPEED !!!
    // !!! IMPORTANT - OPTIMIZE FOR SPEED !!!
    // !!! IMPORTANT - OPTIMIZE FOR SPEED !!!
    template<class data_t, class uint_t>
    void Buffer<data_t, uint_t>::copyToArray(data_t *array, uint_t start, uint_t count)
    {
        // Pointers must be set
        if(m_buffer == nullptr || array == nullptr) return;
        // Buffer must not be empty
        else if(m_buffer_head == m_buffer_tail) return;
        // Can not access elements outside array boundaries
        else if((start+count)>(m_buffer_count-1)) return;

        uint_t last_index = start + count;
        if(count == 0) last_index = m_buffer_count ;

        for(uint_t i = start; i < last_index; ++i) array[i] = m_buffer[(m_buffer_head - 1 - i) & m_buffer_mask];
    }

    template<class data_t, class uint_t>
    data_t &Buffer<data_t, uint_t>::operator[](uint_t index)
    {
        return m_buffer[(m_buffer_head - 1 - index) & m_buffer_mask];
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::operator<<(const data_t &value)
    {
        pushFront(value);
        return *this;
    }

    template<class data_t, class uint_t>
    Buffer<data_t, uint_t> &Buffer<data_t, uint_t>::operator>>(data_t &value)
    {
        popBack(value);
        return *this;
    }
}

#endif // BUFFER_H
