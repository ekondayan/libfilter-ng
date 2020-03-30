# libfilter-ng

---

# Description

The library provides a set of signal filters: High Pass, Low Pass, Moving Average, Moving Average Kaufman, Weighted Moving Average, Moving Median, Interval Average, Interval Median, Interpolation.

There two filters: Most Frequent Occurrence and Moving Middle; that are my own design and I will explain them in more details later on. 

There is also an implementation of a very efficient and self contained Circular Buffer.

The minimum C++ standard is [ISO/IEC 14882](https://en.wikipedia.org/wiki/ISO/IEC_14882) (C++17). It doesn't rely heavily on C++17 and can be easily degraded to C++11.

The main goals of the library are: efficiency and simplicity. It is designed to work on from tiny MCU to desktop computers and servers and anything in between.

The project is hosted on GitHub [https://github.com/ekondayan/libfilter-ng](https://github.com/ekondayan/libfilter-ng).

# Dependencies

It is self contained and does not rely on any external dependencies. It is  just a vanilla C++17 code.

# Usage

From a user perspective, the filters are like black boxes where on the one end you feed them with values and on the other end you get filtered values. Based on the previous assumption,  every filter implements at least two methods: `int()` and  `out()`. 

Example:

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

- Get filtered value

```c++
float result = low_pass.out();
```

The Low Pass and High Pass are one of the simplest filters, because they work only with the new value and the last result. 

On the other hand there are filters that work on a sequence of input values and for that reason they require a storage place. The library implements a self contained and efficient Circular Buffer. Contrary to what you might be thinking, the Circular Buffer object does not manage the memory itself. The memory management is your responsibility. 

To create a Circular Buffer object, you have to pass a pointer to the pre allocated memory. This decision was made, because different scenarios require different memory management. For example, if you are developing for a MCU, you may prefer the memory to be statically allocated. Or if you are developing for a desktop, you may prefer to dynamically allocate space on the heap. If for some reason you prefer to use the stack, you can do it and the library will not stand in your way.


