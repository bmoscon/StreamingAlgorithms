/*
 * kps.hpp
 *
 *
 * Karp-Papadimitriou-Shenker Algorithm Implementation
 * 
 * as introduced in "A Simple Algorithm for Finding Frequent Elements in Streams and Bags"
 *   by Karp, Papadimitriou, and Shenker
 *
 *
 * Copyright (C) 2013  Bryant Moscon - bmoscon@gmail.com
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

#ifndef __KPS__
#define __KPS__

#include <vector>
#include <tr1/unordered_map>
#include <stdint.h>

template <class S>
class KPS {
public:
    
  KPS(const double theta) : theta_(1.0 / theta) {}
        
  void add(const S &x) 
  {
    // insert or incrent phase
    typename std::tr1::unordered_map<S, uint64_t>::iterator map_it = k_.find(x);
    if (map_it == k_.end()) {
      k_.insert(std::make_pair<S, uint64_t>(x, 1));
    } else {
      ++(map_it->second);
    }

    // delete phase
    if (k_.size() > theta_) {
      for (map_it = k_.begin(); map_it != k_.end(); ++map_it) {
	--(map_it->second);
	if (map_it->second == 0) {
	  k_.erase(map_it);
	}
      } 
    }
  }

  void add(const std::vector<S> &x)
  {
    for (int i = 0; i < x.size(); ++i) {
      add(x[i]);
    }
  }

  bool exists(const S &x) const
  {
    return (k_.find(x) != k_.end());
  }
    
  //returning a vector ok thanks to copy elision/return value optimization
  std::vector<S> report() const 
  {
    typename std::tr1::unordered_map<S, uint64_t>::const_iterator it;
    std::vector<S> ret;
    ret.reserve(k_.size());
    
    for (it = k_.begin(); it != k_.end(); ++it) {
      ret.push_back(it->first);
    }

    return (ret);
  }
    
private:
  std::tr1::unordered_map<S, uint64_t> k_;
  double theta_;
};


#endif
