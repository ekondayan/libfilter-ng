# libfilter-ng

---

# Description

The library provides a set of signal filters: High Pass, Low Pass, Moving Average, Moving Average Kaufman, Weighted Moving Average, Moving Median, Interval Average, Interval Median, Interpolation.

There two filters: Most Frequent Occurrence and Moving Middle; that are my own custom design and I will explain them in more details later on. 

There is also an implementation of a very efficient and self contained Circular Buffer.

The minimum C++ standard is [ISO/IEC 14882](https://en.wikipedia.org/wiki/ISO/IEC_14882) (C++17). It doesn't rely heavily on C++17 features and can be easily degraded to C++11.

The main goals of the library are: efficiency and simplicity. It is designed to work on from tiny MCU to desktop computers and servers and anything in between.

The project is hosted on GitHub [https://github.com/ekondayan/libfilter-ng](https://github.com/ekondayan/libfilter-ng).

# Dependencies

It is self contained and does not rely on any external dependencies. It is  just a vanilla C++17 code.

# Usage

From a user perspective, the filters are like black boxes where on the one end you feed them with values and on the other end you get filtered values. Based on the previous assumption,  every filter implements at least two methods: `in()` and  `out()`. Another advantage of this approach is that the data crunching can be split between the two functions. The sole purpose of the `in()` method is to feed the values into the filter as fast as possible and nothing more. The `out()` method is where the actual calculation happens. The actual benefit is gained performance because that the filter will run the computational intensive calculations only when you require them. For example if you have a signal that is generating values 100 times a second but your application use that data only once per second. LowPass, HighPass and IIR filters in general are an exception to this rule because the filtered value is dependent on the previous one. Luckily for LowPass and HighPass are not computationally intensive but nonetheless their equations are rewritten to minimize the use of arithmetic operations.

Low Pass filter example:

- Create a Low Pass filter object. 
  The template parameter is the type of the values the filter will work with.
  The constructor parameter is the filter coefficient, which determines how aggressive the filter will be.

```c++
filter::LowPass<float> low_pass(0.4f);
```

- Feed the filter

```
low_pass.in(2.0);
```

- Get the filtered value

```c++
float result = low_pass.out();
```

The Low Pass and High Pass are one of the simplest filters, because internally they work only with the new value and the last result. 

On the other hand there are filters that work on a sequence of input values and for that reason they require a FILO storage buffer. The library implements a self contained and efficient Circular Buffer. Contrary to what you might be thinking, the Circular Buffer object does not manage the memory itself. The memory management is your responsibility. 

To create a Circular Buffer object, you have to pass a pointer to the pre allocated memory. This decision was made, because different scenarios require different memory management. For example, if you are developing for a MCU, you may prefer the memory to be statically allocated. Or if you are developing for a desktop, you may prefer to dynamically allocate space on the heap. If for some reason you prefer to use the stack, you can do it and the library will not stand in your way.

Circular buffer is optimized for speed. Pushing, popping and rotating use cheap arithmetic operations, no modulus operator or conditional branching. For this to be achieved the buffer size must be a power of two(2, 4, 8, 16, 32, ...). Since this is circular buffer the real size is always the set value minus one. One element is wasted for the head pointer. For example if you set the buffer size to 16 elements, then the real size is 15 elements. 

Moving Median example

- Allocate memory for the circular buffer

```c++
float buff_mov_med[4] = {0.0f};
```

- Create the Median Filter object. The template parameter is the type of the values the filter will work with. The first argument to the constructor is a pointer to the allocated memory. The second is the size of the buffer

```c++
filter::MovingMedian<float> mov_med(buff_mov_med, 4);
```

- Feed the filter. Calculating the median requires at least 3 values

```
mov_med.in(2.0);
mov_med.in(2.5);
mov_med.in(3.0);
```

- Get the filtered value

```c++
float result = mov_med.out();
```

# Filters

## Interval Median

## Interval Average

## Interpolation



## Low Pass

Passes the low frequency part of the signal and attenuates the high frequency part of the signal

## High Pass

Passes the high frequency part of the signal and attenuates the low frequency part of the signal

## Moving Average

## Moving Exponential Average

## Moving Weighted Average

## Moving Kaufman Average

## Moving Median

## Moving Most Frequent Occurrence

Keeps track of the number of times a value shows in the buffer or in other words - the frequency of occurrence of each vale.

This filter require two memory buffers:

- one for the signal values themselves

- another for keeping track of the values and their occurrences

Example:

- Allocate memory for the circular buffer

```c++
float buff_mfo[4] = {0.0f};
```

- Allocate memory for the second buffer

```c++
filter::MovingMostFrequentOccurrence<float>::Occurrence buf_occ_mfo[4];
```

- Create the object. The template parameter is the type of the values the filter will work with. The first argument to the constructor is a pointer to the allocated memory. The second is the size of the buffer

```c++
filter::MovingMostFrequentOccurrence<float> mfo(buf_mfo, buf_occ_mfo, 4);
```

- Feed the filter. Calculating the median requires at least 3 values

```
mfo.in(2.0);
mfo.in(2.0);
mfo.in(3.0);
```

- Get the filtered value

```c++
float result = mfo.out();
```

## Moving Middle

Takes the minimum and maximum values in the buffer, then calculates the arithmetic middle and selects the closes value from the buffer.








