/*
 * spectral_bloom.hpp
 *
 *
 * Spectral Bloom Filter Implementation
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

#ifndef __SPECTRAL_BLOOM_FILTER__
#define __SPECTRAL_BLOOM_FILTER__

#include <limits>
#include <stdint.h>

#include "counting_bloom.hpp"


template <class S, class T>
class SpectralBloomFilter : public CountingBloomFilter<S, T> {
public:
  typedef S (*hash_function)(const T &s);
    
    
  SpectralBloomFilter(const std::vector<hash_function> &hash_list, uint32_t bits) : 
    CountingBloomFilter<S,T>(hash_list, bits) {}

  SpectralBloomFilter(const CountingBloomFilter<S,T> &s) : 
    CountingBloomFilter<S,T>(s) {}
        
  
  uint64_t occurrences(const T &s) const {
    
    uint64_t return_val = std::numeric_limits<uint64_t>::max();
    uint64_t compare_val;
    
    for (uint32_t i = 0; i < parent::hash_list_.size(); ++i) {
      compare_val = parent::bloom_array_.at((*parent::hash_list_[i])(s));
      if (compare_val < return_val) {
	return_val = compare_val;
      }
    }
    
    return (return_val);
  }

private:
  typedef CountingBloomFilter<S,T> parent;

};


#endif
