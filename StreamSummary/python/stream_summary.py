"""
Copyright (C) 2012-2013  Bryant Moscon - bmoscon@gmail.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to 
 deal in the Software without restriction, including without limitation the 
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 1. Redistributions of source code must retain the above copyright notice, 
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution, and in the same 
    place and form as other copyright,
    license and disclaimer information.

 3. The end-user documentation included with the redistribution, if any, must 
    include the following acknowledgment: "This product includes software 
    developed by Bryant Moscon (http://www.bryantmoscon.org/)", in the same 
    place and form as other third-party acknowledgments. Alternately, this 
    acknowledgment may appear in the software itself, in the same form and 
    location as other such third-party acknowledgments.

 4. Except as contained in this notice, the name of the author, Bryant Moscon,
    shall not be used in advertising or otherwise to promote the sale, use or 
    other dealings in this Software without prior written authorization from 
    the author.


 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 THE SOFTWARE.

"""

class Bucket(object):
    def __init__(self, v):
        self.val = v
        self.items = []

    def __lt__(self, other):
        return self.val < other.value()

    def __str__(self):
        s = "Bucket " + str(self.val) + "\n"
        s += str(self.items)
        return s

    def insert(self, e):
        self.items.append(e)
    
    def min(self):
        return self.items[0]

    def size(self):
        return len(self.items)

    def remove(self, e):
        self.items.remove(e)

    def value(self):
        return self.val
        


class StreamSummary(object):
    def __init__(self, size):
        self.size = size
        # maps items to buckets
        self.bucket_map = {}
        # sorted list of buckets (in order by value)
        self.bucket_list = []

        self.bucket_list.append(Bucket(1))

        
    def __str__(self):
        s = ""
        for b in self.bucket_list:
            s += str(b) 
            s += "\n"
        return s

    def add(self, e):
        if e in self.bucket_map:
            b = self.bucket_map[e]
            v = b.value()
            
            b.remove(e)
            if b.size() == 0:
                self.bucket_list.remove(b)
            
            for i in self.bucket_list:
                if i.value() == v + 1:
                    i.insert(e)
                    self.bucket_map[e] = i
                    return
            
            b = Bucket(v + 1)
            b.insert(e)
            self.bucket_map[e] = b
            self.bucket_list.append(b)
            self.bucket_list.sort()
        else:
            if len(self.bucket_map.keys()) < self.size:
                if self.bucket_list[0].value() > 1:
                    self.bucket_list.insert(0, Bucket(1))
                    self.bucket_list[0].insert(e)
                    self.bucket_map[e] = self.bucket_list[0]
                else:
                    self.bucket_map[e] = self.bucket_list[0]
                    self.bucket_list[0].insert(e)
            else:
                ejected = self.bucket_list[0].min()
                ejected_val = self.bucket_list[0].value()
                del self.bucket_map[ejected]
                self.bucket_list[0].remove(ejected)
                if self.bucket_list[0].size() == 0:
                    del self.bucket_list[0]

                for i in self.bucket_list:
                    if i.value() == ejected_val + 1:
                        i.insert(e)
                        self.bucket_map[e] = i
                        return

                b = Bucket(ejected_val + 1)
                b.insert(e)
                self.bucket_list.append(b)
                self.bucket_list.sort()
                self.bucket_map[e] = b

