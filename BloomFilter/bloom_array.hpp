/*
 * bloom_array.hpp
 *
 *
 * Bloom Filter Array Implementation
 *
 *
 * Copyright (C) 2012  Bryant Moscon - bmoscon@gmail.com
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
 *    place and form as other copyright,
 *    license and disclaimer information.
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

static uint64_t mask_table[] = {0x1,    0x3,    0x7,    0xF,   
				0x1F,   0x3F,   0x7F,   0xFF, 
				0x1FF,  0x3FF,  0x7FF,  0xFFF, 
				0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};

template <class T = uint64_t>
class BloomArray {

public:

  BloomArray(const T &elements, const T &bits_per_element = 1) :
    array_(ceil(elements * bits_per_element / (sizeof(T) * 8.0)), 0),
    elem_bits_(bits_per_element),
    elem_mask_(mask_table[bits_per_element-1]) { 
    
    assert(bits_per_element < 17 && bits_per_element); 
  }

  T at(const T &index) const {

    T idx = index_translate(index);
    T mask = elem_mask_ << (bitcount() - idx);

    return (lookup(idx, mask));
  }

  void inc(const T &index) {
    T idx = index_translate(index);
    T mask = elem_mask_;
    
    mask <<= (bitcount() - idx);

    T value = lookup(idx, mask);
    
    if (value == elem_mask_) {
      return;
    } else {
      array_[idx >> (log2(bitcount()))] &= ~mask;
      
      ++value;
      value <<= (bitcount() - idx);

      array_[idx >> (log2(bitcount()))] |= value;
    }
  }

  void dec(const T &index) {
    T idx = index_translate(index);
    T mask = elem_mask_;
    
    mask <<= (bitcount() - idx);

    T value = lookup(idx, mask);

    if (value == 0) {
      return;
    } else {
      array_[idx >> (log2(bitcount()))] &= ~mask;

      --value;
      value <<= ((sizeof(T) << 3) - idx);

      array_[idx >> (log2(bitcount()))] |= value;
    }
  }

  void dump() {
    for (int i = 0; i < array_.size(); ++i) {
      std::cout << array_[i] << std::endl;
    }
  }


protected:

  inline T index_translate(const T &index) const {
    return ((index + 1) * elem_bits_);
  }

  inline T lookup(const T &index, T &mask) const {
    return ((array_[index >> log2(bitcount())] & mask) >> ((sizeof(T) << 3) - index));
  }

  inline T bitcount() const {
    return (sizeof(T) << 3);
  }

  inline uint32_t log2(const uint32_t x) {
    uint32_t ret;
    
    asm ( "\tbsr %1, %0\n"
	  : "=r" (ret)
	  : "r"  (x)
        );
    
    return (ret);
  }
  
  
  std::vector<T> array_;
  T elem_bits_;
  T elem_mask_;
};


#endif
