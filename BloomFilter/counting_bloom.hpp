/*
 * counting_bloom.hpp
 *
 *
 * Counting Bloom Filter Implementation
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

#ifndef __COUNTING_BLOOM_FILTER__
#define __COUNTING_BLOOM_FILTER__

#include <vector>
#include <limits>
#include <stdint.h>


#include "bloom_.hpp"

template <class S, class T, class U>
class CountingBloomFilter : Bloom<S, T> {
public:
  typedef S (*hash_function)(const T &s);
    
    
  CountingBloomFilter(const std::vector<hash_function> &hash_list) : 
    bloom_array_(std::numeric_limits<S>::max(), false), 
    hash_list_(hash_list) {}
        
  virtual void add(const T &s) {
    for (uint32_t i = 0; i < hash_list_.size(); ++i) {
      ++bloom_array_[(*hash_list_[i])(s)];
    }
  }

  virtual void remove(const T &s) {
    for (uint32_t i = 0; i < hash_list_.size(); ++i) {
      --bloom_array_[(*hash_list_[i])(s)];
    }
  }

    
  virtual bool exists(const T &s) const {
    for (uint32_t i = 0; i < hash_list_.size(); ++i) {
      if (!bloom_array_[(*hash_list_[i])(s)]) {
	return (false);
      }
    }
    
    return (true);
  }
    
private:
  
  std::vector<U> bloom_array_;
  std::vector<hash_function> hash_list_;
};


#endif
