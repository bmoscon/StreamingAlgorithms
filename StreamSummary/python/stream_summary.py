"""
Copyright (C) 2012-2015  Bryant Moscon - bmoscon@gmail.com
 
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

from bisect import bisect_left
from itertools import chain


class Bucket(object):
    '''
    A bucket is a list of objects with the same value
    '''
    
    def __init__(self, v):
        self.val = v
        self.items = []

        
    def __lt__(self, other):
        # need to check if other is an int (for bisect left) or
        # another bucket object (for when it is sorted)
        if type(other) is int:
            return self.val < other
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
    '''
    Summarizes a Stream into the top N-1 number of objects (where N == SIZE). The last object 
    in the StreamSummary (i.e. the Nth object) should be ignored.
    '''
    def __init__(self, size):
        self.size = size
        # maps buckets to items -- bucket_value (int) : list of items
        self.bucket_map = {}
        # sorted list of buckets (in order by value)
        self.bucket_list = []
        # first item added will go into bucket 1, so add it now
        self.bucket_list.append(Bucket(1))

        
    def __str__(self):
        s = ""
        for b in self.bucket_list:
            s += str(b) 
            s += "\n"
        return s


    def __find_bucket(self, value):
        end = len(self.bucket_list)
        index = bisect_left(self.bucket_list, value, 0, end)
        return index if index != end and self.bucket_list[index].value() == value else None


    def clear(self):
        '''
        Resets the Stream Summary object to its starting state
        '''
        self.bucket_map = {}
        self.bucket_list = []
        self.bucket_list.append(Bucket(1))

    
    def to_value_dict(self):
        '''
        Returns a dictionary of value -> list of objects that share this value
        '''
        ret = {}
        for b in self.bucket_list:
            for i in b.items:
                ret[i] = b.value()
        return ret

    
    def to_item_dict(self):
        '''
        Returns a dictionary of item -> item value
        '''
        ret = {}
        for b in self.bucket_list:
            ret[b.value()] = b.items.copy()
        return ret

    
    def to_list(self):
        '''
        Returns a list of the items in the StreamSummary object
        '''
        return list(chain.from_iterable([b.items for b in self.bucket_list]))


    def exists(self, item):
        '''
        Checks if item exists in the current top seen items.
        Note: this can be slow for large sized StreamSummary objects.
        Worst case O(n^2), but can be made O(n) by using more memory
        (add in a dict that maps items -> values)
        '''
        for b in self.bucket_list:
            if item in b.items:
                return True
        return False
    

    def add(self, e):
        '''
        Adds an item to the StreamSummary object
        '''
        # first check to see if item already present
        if e in self.bucket_map:
            # if so, remove it from old bucket
            b = self.bucket_map[e]
            v = b.value()
            
            b.remove(e)
            # if old bucket is empty, we can remove it
            if b.size() == 0:
                self.bucket_list.remove(b)
            # find new bucket and insert there using binary search
            bucket_index = self.__find_bucket(v + 1)
            if bucket_index is not None:
                self.bucket_list[bucket_index].insert(e)
                self.bucket_map[e] = self.bucket_list[bucket_index]
            else:
                # new bucket of value <old value + 1> doesnt exist
                # so create it now
                b = Bucket(v + 1)
                b.insert(e)
                self.bucket_map[e] = b
                self.bucket_list.append(b)
                self.bucket_list.sort()
        else:
            # value not present, so lets check if we can just add it
            if len(self.bucket_map.keys()) < self.size:
                # we can (havent reached size yet)
                if self.bucket_list[0].value() > 1:
                    self.bucket_list.insert(0, Bucket(1))
                    self.bucket_list[0].insert(e)
                    self.bucket_map[e] = self.bucket_list[0]
                else:
                    self.bucket_map[e] = self.bucket_list[0]
                    self.bucket_list[0].insert(e)
            else:
                # we're full, so to add this we need to eject a value
                ejected = self.bucket_list[0].min()
                ejected_val = self.bucket_list[0].value()
                del self.bucket_map[ejected]
                self.bucket_list[0].remove(ejected)
                # if the bucket is empty now, we can remove it
                if self.bucket_list[0].size() == 0:
                    del self.bucket_list[0]
                
                # look up new bucket
                bucket_index = self.__find_bucket(ejected_val + 1)
                if bucket_index is not None:
                    self.bucket_list[bucket_index].insert(e)
                    self.bucket_map[e] = self.bucket_list[bucket_index]
                else:
                    b = Bucket(ejected_val + 1)
                    b.insert(e)
                    self.bucket_list.append(b)
                    self.bucket_list.sort()
                    self.bucket_map[e] = b

