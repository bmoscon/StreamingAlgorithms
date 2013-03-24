/*
 * bloom_array.hpp
 *
 *
 * Bloom Filter Array Implementation
 *
 *
 * Copyright (C) 2012-2013  Bryant Moscon - bmoscon@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution, and in the same 
 *    place and form as other copyright, license and disclaimer information.
 *
 * 3. The end-user documentation included with the redistribution, if any, must 
 *    include the following acknowledgment: "This product includes software 
 *    developed by Bryant Moscon (http://www.bryantmoscon.org/)", in the same 
 *    place and form as other third-party acknowledgments. Alternately, this 
 *    acknowledgment may appear in the software itself, in the same form and 
 *    location as other such third-party acknowledgments.
 *
 * 4. Except as contained in this notice, the name of the author, Bryant Moscon,
 *    shall not be used in advertising or otherwise to promote the sale, use or 
 *    other dealings in this Software without prior written authorization from 
 *    the author.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */
 
#ifndef __BLOOM_FILTER_ARRAY__
#define __BLOOM_FILTER_ARRAY__

#include <stdint.h>
#include <vector>
#include <cmath>
#include <cassert>

template <class T = uint64_t>
class BloomArray {

public:

  BloomArray(const T &elements, const T &bits_per_element = 1) :
    array_(ceil(elements * bits_per_element / (sizeof(T) * 8.0)), 0),
    elem_bits_(bits_per_element),
    elem_mask_(pow(2, bits_per_element) - 1),
    bitcount_(sizeof(T) << 3),
    len_(elements)
  { 
    assert(bits_per_element);
    assert(sizeof(T) * 8 % bits_per_element == 0);
    index_shift_ = log2(bitcount_);
  }

  T at(const T &index) const {
    T idx = index_translate(index);
    T mask = elem_mask_ << (bitcount_ - idx);

    return (lookup(idx, mask));
  }

  void inc(const T &index) {
    T idx = index_translate(index);
    T mask = elem_mask_;
    
    mask <<= (bitcount_ - idx);

    T value = lookup(idx, mask);
    
    if (value == elem_mask_) {
      return;
    } else {
      array_[idx >> index_shift_] &= ~mask;
      
      ++value;
      value <<= (bitcount_ - idx);

      array_[idx >> index_shift_] |= value;
    }
  }

  void dec(const T &index) {
    T idx = index_translate(index);
    T mask = elem_mask_;
    
    mask <<= (bitcount_ - idx);

    T value = lookup(idx, mask);

    if (value == 0) {
      return;
    } else {
      array_[idx >> index_shift_] &= ~mask;

      --value;
      value <<= (bitcount_ - idx);

      array_[idx >> index_shift_] |= value;
    }
  }

  void set(const T &index, T value) {
    if (value > elem_mask_) {
      return;
    }
    
    T idx = index_translate(index);
    T mask = elem_mask_;
    
    mask <<= (bitcount_ - idx);
    array_[idx >> index_shift_] &= ~mask;

    value <<= (bitcount_ - idx);
    array_[idx >> index_shift_] |= value;
  }

  T size() const {
    return (len_);
  }

  void dump() {
    for (T i = 0; i < array_.size(); ++i) {
      std::cout << array_[i] << std::endl;
    }
  }


protected:

  inline T index_translate(const T &index) const { 
    return (index << log2(elem_bits_));
  }

  inline T lookup(const T &index, T &mask) const {
    return ((array_[index >> index_shift_] & mask) >> (bitcount_ - index));
  }

  inline uint32_t log2(const uint32_t x) const {
    uint32_t ret;
    
    asm ( "\tbsr %1, %0\n"
	  : "=r" (ret)
	  : "r"  (x)
        );
    
    return (ret);
  }
  
  // Array that holds the underlying datatype elements
  std::vector<T> array_;
  // number of bits per element
  T elem_bits_; 
  // mask to select each element
  T elem_mask_;
  // # of bits in the underlying datatype
  T bitcount_;
  // log2 of bitcount, used in shifting the index
  uint32_t index_shift_;
  // user defined len
  T len_;
};


#endif
