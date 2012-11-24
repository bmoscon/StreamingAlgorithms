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

uint64_t mask_table[] = {0x1,    0x3,    0x7,    0xF,   
                         0x1F,   0x3F,   0x7F,   0xFF, 
	                 0x1FF,  0x3FF,  0x7FF,  0xFFF, 
                         0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};

class BloomArray {

public:

  BloomArray(const uint64_t &elements, const uint64_t &bits_per_element = 1) :
    array_(ceil(elements * bits_per_element / 64.0), 0),
    elem_bits_(bits_per_element),
    elem_mask_(mask_table[bits_per_element-1]) { 
    
    assert(bits_per_element < 17 && bits_per_element); 
  }

  uint64_t at(const uint64_t &index) const {

    uint64_t idx = index_translate(index);
    uint64_t mask = elem_mask_ << (64 - idx);

    return (lookup(idx, mask));
  }

  void inc(const uint64_t &index) {
    uint64_t idx = index_translate(index);
    uint64_t mask = elem_mask_;
    
    mask <<= (64 - idx);

    uint64_t value = lookup(idx, mask);
    
    if (value == elem_mask_) {
      return;
    } else {
      array_[idx >> 6] &= ~mask;
      
      ++value;
      value <<= (64 - idx);

      array_[idx >> 6] |= value;
    }
  }

  void dec(const uint64_t &index) {
    uint64_t idx = index_translate(index);
    uint64_t mask = elem_mask_;
    
    mask <<= (64 - idx);

    uint64_t value = lookup(idx, mask);

    if (value == 0) {
      return;
    } else {
      array_[idx >> 6] &= ~mask;

      --value;
      value <<= (64 - idx);

      array_[idx >> 6] |= value;
    }
  }


protected:

  inline uint64_t index_translate(const uint64_t &index) const {
    return ((index + 1) * elem_bits_);
  }

  inline uint64_t lookup(const uint64_t &index, uint64_t &mask) const {
    return ((array_[index >> 6] & mask) >> (64 - index));
  }
  
  
  std::vector<uint64_t> array_;
  uint64_t elem_bits_;
  uint64_t elem_mask_;
};


#endif
