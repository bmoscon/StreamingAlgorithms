/*
 * ss_test.cc
 *
 *
 * Stream Summary Test
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
 */

#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <sstream>

#include "../stream_summary.hpp"


std::string read_file(const char* file)
{
    std::ifstream file_h(file);
    std::string data;
    
    if (file_h.is_open()) {
	file_h.seekg(0, std::ios::end);
	data.resize(file_h.tellg());
	file_h.seekg(0, std::ios::beg);
	file_h.read(&data[0], data.size());
	file_h.close();
    } else {
	perror("error");
	exit(1);
    }
    
    return(data);
}


int main(int argc, char* argv[])
{
    std::string line;
    std::string data;
    
    if (argc != 3) {
	std::cout << "usage: test_ss <filter_size> <input file>" << std::endl;
	exit(1);
    }
    
    StreamSummary<std::string> a(atoi(argv[1]));
    
    data = read_file(argv[2]);
    std::stringstream stream(data);
    
    
    while (std::getline(stream, line)) {
	if (!line.size()) {
	    // blank lines (i.e. lines with only a carriage return) will have line with len 0
	    continue;
	}
	if (line[0] != '#') {
	    a.add(line);
	}
    }
    std::cout << std::endl;
    
    std::cout << "printing object:\n";
    std::cout << a;
    
    std::cout << "top object vector:\n";
    std::vector<std::string> v = a.to_vector();
    
    for (unsigned int i = 0; i < v.size(); ++i) {
	std::cout<< v[i] << " ";
    }
    std::cout << std::endl;
    
    return (0);
}
