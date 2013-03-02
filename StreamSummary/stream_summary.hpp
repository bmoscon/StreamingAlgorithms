/*
 * stream_summary.hpp
 *
 *
 * Stream Summary Algorithm Implementation
 * 
 * as introduced in "Efficient Computation of Frequent and Top-k Elements in Data Streams"
 *   by A. Metwally, D. Agrawal, and E. Abbadi
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

#ifndef __STREAM_SUMMARY__
#define __STREAM_SUMMARY__

#include <stdint.h>
#include <vector>
#include <cassert>
#include <tr1/unordered_map>
#include <map>

template <class T>
class Bucket {
public:
  Bucket(const uint64_t &v = 1) : value(v) {}
  
  void insert(const T &obj)
  {
    list.push_back(obj);
  }

  void remove(const T &obj)
  {
    for (uint64_t i = 0; i < list.size(); ++i) {
      if (list[i] == obj) {
	list.erase(list.begin()+i);
	break;
      }
    }
  }

  uint64_t getValue() const
  {
    return (value);
  }

  size_t getSize() const
  {
    return (list.size());
  }

  T getMin() const
  {
    assert(list.size() > 0);
    return (list[0]);
  }

  void print() const 
  {
    std::cout << value << std::endl;
    for (uint64_t i = 0; i < list.size(); ++i) {
      std::cout << list[i] << " ";
    }
    std::cout << std::endl << std::endl;;
  }

private:
  uint64_t value;
  std::vector<T> list;
};


template <class T>
class StreamSummary
{
public:
  StreamSummary(const uint64_t &size) : max_size(size), size(0) {}

  void add(const T &obj)
  {
    bm_ret ret;
    
    ret = exists(obj);

    if (ret.second) {
      increment(obj, ret.first);
    } else if (size < max_size) {
      insert(obj);
    } else {
      replace_and_insert(obj);
    }
  }

  void print() const
  {
    typename std::map<uint64_t, Bucket<T> *>::const_iterator it;
    for (it = value_map.begin(); it != value_map.end(); ++it) {
      it->second->print();
    }
    
  }


private:
  typedef typename std::tr1::unordered_map<T, Bucket<T> *>::iterator bm_it;
  typedef typename std::map<uint64_t, Bucket<T> *>::iterator v_it;
  typedef std::pair<bm_it, bool> bm_ret;
  

  bm_ret exists(const T &obj)
  {
    bm_it it = bucket_map.find(obj);
    
    if (it == bucket_map.end()) {
      return (std::make_pair(it, false));
    }

    return (std::make_pair(it, true));
  }
  
  void increment(const T &obj, bm_it &it)
  {
    Bucket<T> *b = it->second;
    uint64_t val = b->getValue();

    // regardless of what path we take below, we need to remove the old entry 
    // from the original bucket
    b->remove(obj);
    // and remove the entry from that T to the bucket, since it is going to a new bucket
    bucket_map.erase(it);
    // if bucket is now empty, we can remove it
    if (b->getSize() == 0) {
      value_map.erase(val);
      delete b;
    }
    
    // check if bucket of val+1 exists
    v_it value_it = value_map.find(val + 1);

    if (value_it != value_map.end()) {
      // bucket exists, insert object
      value_it->second->insert(obj);
    } else {
      // does not exits, create new bucket
      bm_ret ret;
      ret = bucket_map.insert(std::make_pair(obj, new Bucket<T>(val+1)));
      assert(ret.second);
      value_map.insert(std::make_pair(val + 1, ret.first->second));
      ret.first->second->insert(obj);
    }

  }
  
  void insert(const T &obj)
  {
    bm_ret ret;
    v_it value_it;

    value_it = value_map.find(1);

    // check if bucket with value 1 exists
    if (value_it == value_map.end()) {
      ret = bucket_map.insert(std::make_pair(obj, new Bucket<T>(1)));
      assert(ret.second);
      value_map.insert(std::make_pair(1, ret.first->second));
      ret.first->second->insert(obj);
    } else {
      value_it->second->insert(obj);
      bucket_map.insert(std::make_pair(obj, value_it->second));
    }
    
    ++size;
  }
  
  void replace_and_insert(const T &obj)
  {
    Bucket<T> *b = value_map.begin()->second;
    uint64_t val = b->getValue();
    T old = b->getMin();

    // remove old object
    b->remove(old);
    bucket_map.erase(old);

    // check size of old bucket
    if (b->getSize() == 0) {
      value_map.erase(val);
      delete b;
    }

    // check if bucket of val+1 exists
    v_it value_it;
    value_it = value_map.find(val + 1);
    if (value_it != value_map.end()) {
      // bucket exists, insert object
      value_it->second->insert(obj);
      // insert obj into map
      bucket_map.insert(std::make_pair(obj, value_it->second));
    } else {
      // does not exits, create new bucket
      bm_ret ret;
      ret = bucket_map.insert(std::make_pair(obj, new Bucket<T>(val+1)));
      assert(ret.second);
      value_map.insert(std::make_pair(val + 1, ret.first->second));
      ret.first->second->insert(obj);
    }
  }
  
  
  uint64_t max_size;
  uint64_t size;
  std::tr1::unordered_map<T, Bucket<T> *> bucket_map;
  std::map<uint64_t, Bucket<T> *> value_map;
  
};



#endif
