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
 * Copyright (C) 2013-2015  Bryant Moscon - bmoscon@gmail.com
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
#include <unordered_map>
#include <map>
#include <list>
#include <iostream>


template <class T>
class Bucket {
public:
    Bucket(const uint64_t &v = 1) : value_(v) {}
    
    void insert(const T &obj)
    {
	list_.push_back(obj);
    }
    
    void remove(const T &obj)
    {
	typename std::list<T>::iterator it;
	
	for (it = list_.begin(); it != list_.end(); ++it) {
	    if (*it == obj) {
		list_.erase(it);
		return;
	    }
	}
	
	// we should only arrive here if obj is not in the list, which should never happen
	// so throw an error
	assert(false);
    }
    
    uint64_t value() const
    {
	return (value_);
    }
    
    size_t size() const
    {
	return (list_.size());
    }
    
    T min() const
    {
	assert(list_.size() > 0);
	return (list_.front());
    }

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const Bucket<U>& b);
    
private:    
    uint64_t value_;
    std::list<T> list_;
};


template <class T>
class StreamSummary
{
public:
    StreamSummary(const uint64_t &size) : max_size(size) {}
    
    ~StreamSummary()
    {
	clear();
    }
    
    void add(const T &obj)
    {
	bm_ret ret;
	
	ret = exists(obj);
	
	if (ret.second) {
	    increment(obj, ret.first);
	} else if (bucket_map.size() < max_size) {
	    insert(obj);
	} else {
	    replace_and_insert(obj);
	}
    }
    
    bool exists(const T &obj) const
    {
	typename std::unordered_map<T, Bucket<T> *>::const_iterator it;
	
	it = bucket_map.find(obj);
	
	return (it != bucket_map.cend());
    }
    
    // thanks to copy elision/RVO the compiler will
    // elide the copy, so no performance hit
    // in returning a copy of the local variable
    std::vector<T> to_vector() const
    {
	std::vector<T> ret;
	typename std::unordered_map<T, Bucket<T> *>::const_iterator it;
	
	for (it = bucket_map.begin(); it != bucket_map.end(); ++it) {
	    ret.push_back(it->first);
	}
	
	return (ret);
    }
    
    void clear ()
    {
	v_it vit;
	Bucket<T> *b;
	
	vit = value_map.begin(); 
	while (vit != value_map.end()) {
	    b = vit->second;
	    vit = value_map.erase(vit);
	    delete b;
	}
	
	bucket_map.clear();
    }

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const StreamSummary<U>& s);
    
private:
    typedef typename std::unordered_map<T, Bucket<T> *>::iterator bm_it;
    typedef typename std::map<uint64_t, Bucket<T> *>::iterator v_it;
    typedef std::pair<bm_it, bool> bm_ret;
    typedef std::pair<v_it, bool> v_ret;
    
    
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
	uint64_t val = b->value();
	
	// regardless of what path we take below, we need to remove the old entry 
	// from the original bucket
	b->remove(obj);
	// if bucket is now empty, we can remove it
	if (b->size() == 0) {
	    value_map.erase(val);
	    delete b;
	}
	
	// check if bucket of val+1 exists
	v_it value_it = value_map.find(val + 1);
	v_ret value_ret;
	
	if (value_it != value_map.end()) {
	    // bucket exists, insert object
	    value_it->second->insert(obj);
	    it->second = value_it->second;
	} else {
	    // does not exits, create new bucket
	    it->second = new Bucket<T>(val+1);
	    value_ret = value_map.insert(std::make_pair(val + 1, it->second));
	    assert(value_ret.second);
	    it->second->insert(obj);
	}
    }
    
    void insert(const T &obj)
    {
	bm_ret ret;
	v_it value_it;
	v_ret value_ret;
	
	value_it = value_map.find(1);
	
	// check if bucket with value 1 exists
	if (value_it == value_map.end()) {
	    ret = bucket_map.insert(std::make_pair(obj, new Bucket<T>(1)));
	    assert(ret.second);
	    value_ret = value_map.insert(std::make_pair(1, ret.first->second));
	    assert(value_ret.second);
	    ret.first->second->insert(obj);
	} else {
	    value_it->second->insert(obj);
	    ret = bucket_map.insert(std::make_pair(obj, value_it->second));
	    assert(ret.second);
	}
    }
    
    void replace_and_insert(const T &obj)
    {
	Bucket<T> *b = value_map.begin()->second;
	uint64_t val = b->value();
	T old = b->min();
	
	// remove old object
	b->remove(old);
	bucket_map.erase(old);
	
	// check size of old bucket
	if (b->size() == 0) {
	    value_map.erase(val);
	    delete b;
	}
	
	// check if bucket of val+1 exists
	v_it value_it;
	bm_ret ret;
	v_ret value_ret;
	value_it = value_map.find(val + 1);
	if (value_it != value_map.end()) {
	    // bucket exists, insert object
	    value_it->second->insert(obj);
	    // insert obj into map
	    ret = bucket_map.insert(std::make_pair(obj, value_it->second));
	    assert(ret.second);
	} else {
	    // does not exits, create new bucket
	    ret = bucket_map.insert(std::make_pair(obj, new Bucket<T>(val+1)));
	    assert(ret.second);
	    value_ret = value_map.insert(std::make_pair(val + 1, ret.first->second));
	    assert(value_ret.second);
	    ret.first->second->insert(obj);
	}
    }
    
    
    uint64_t max_size;
    std::unordered_map<T, Bucket<T> *> bucket_map;
    std::map<uint64_t, Bucket<T> *> value_map;
};


template <class T>
std::ostream& operator<<(std::ostream& os, const Bucket<T>& b)
{
    os << b.value() << "-> ";
	
    typename std::list<T>::const_iterator it;
    for (it = b.list_.begin(); it != b.list_.end(); ++it) {
	os << *it << " ";
    }

    return (os);
}


template <class T>
std::ostream& operator<<(std::ostream& os, const StreamSummary<T>& s)
{
    typename std::map<uint64_t, Bucket<T> *>::const_iterator it;
    for (it = s.value_map.begin(); it != s.value_map.end(); ++it) {
	os << *(it->second);
	os << std::endl << std::endl;
    }
    
    return (os);
}


#endif
