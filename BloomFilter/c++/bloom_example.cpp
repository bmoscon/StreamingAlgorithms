/*
 * bloom_example.cpp
 *
 *
 * Bloom Filter - Example Usage
 *
 *
 * Copyright (C) 2012-2017  Bryant Moscon - bmoscon@gmail.com
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
 *    developed by Bryant Moscon (http://www.bryantmoscon.com/)", in the same 
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
 

#include <iostream>
#include <cstring>

#include "bloom.hpp"
#include "counting_bloom.hpp"
#include "spectral_bloom.hpp"
#include "MurmurHash3.hpp"


uint16_t hash(const std::string &s) {
    uint32_t ret;
    uint16_t rc;
    MurmurHash3_x86_32(s.c_str(), s.length(), 5, &ret);

    memcpy(&rc, &ret, sizeof(uint16_t));

    return rc;
}

int main () {
    std::vector<BloomFilter<uint16_t, std::string>::hash_function> list(1);
    std::vector<CountingBloomFilter<uint16_t, std::string>::hash_function> list2(1);
  
    list[0] = hash;
    list2[0] = hash;
    
    BloomFilter<uint16_t, std::string>  a(list);
    std::string b = "abc";
    
    std::cout << "Testing Empty Bloom Filter for string \"" << b << "\"\n"; 
    std::cout << a.exists(b) << std::endl << std::endl;

    
    std::cout << "Adding string \"" << b << "\" to Bloom Filter and testing for existence\n"; 
    a.add(b);

    std::cout << a.exists(b) << std::endl << std::endl;
    
    

    CountingBloomFilter<uint16_t, std::string> count_b(list2, 4);
    
    count_b.add(b);
    count_b.add(b);
    count_b.remove(b);
    std::cout << "Adding string \"" << b << "\" to Counting Bloom Filter twice, and removing "
        << "once, then testing for existence\n"; 
    std::cout << count_b.exists(b) << std::endl << std::endl;


    std::cout << "Adding string \"" << b << "\" to Spectral Bloom Filter three times and "
        << "removing once, then getting occurrences\n"; 
    SpectralBloomFilter<uint16_t, std::string> spectral_b(list2, 4);

    spectral_b.add(b);
    spectral_b.add(b);
    spectral_b.add(b);
    spectral_b.remove(b);
    
    std::cout << spectral_b.occurrences(b) << std::endl << std::endl;

    SpectralBloomFilter<uint16_t, std::string> spectral_b2(count_b);
    std::cout << "Copying previous Counting Bloom Filter into new Spectral Bloom Filter and "
        << "testing existence\n"; 

    std::cout << spectral_b2.exists(b) << std::endl << std::endl;
    
    return 0;
}
