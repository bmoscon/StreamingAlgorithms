"""
Copyright (C) 2012-2016  Bryant Moscon - bmoscon@gmail.com
 
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
    developed by Bryant Moscon (http://www.bryantmoscon.com/)", in the same 
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
    '''
    A bucket is a list of objects with the same value
    '''
    
    def __init__(self, v):
        self._val = v
        self._items = []

    def __str__(self):
        s = "Bucket " + str(self._val) + "\n"
        s += str(self._items)
        return s

    def insert(self, e):
        self._items.append(e)

    def oldest(self):
        return self._items[0]

    def size(self):
        return len(self._items)

    def remove(self, e):
        self._items.remove(e)

    def value(self):
        return self._val

    def items(self):
        return self._items


class StreamSummary(object):
    '''
    Summarizes a Stream into the top N-1 number of objects (where N == SIZE). The last object 
    in the StreamSummary (i.e. the Nth object) should be ignored.
    '''
    def __init__(self, size):
        self.size = size
        # maps bucket value -> bucket
        self.bucket_map = {}
        # maps item -> bucket
        self.item_map = {}
        # keeps track of the minimum valued bucket
        self.min_val = 0

    def __str__(self):
        s = ''
        for key in self.bucket_map.keys():
            s += str(self.bucket_map[key])
            s += '\n'
        return s
    
    def __increment(self, item):
        '''
        item exists, so remove it from its current bucket, and
        insert it into bucket+1
        '''
        # get value of item, and remove the item from that bucket.
        # if bucket is empty, remove it
        b = self.item_map[item]
        val = b.value()
        b.remove(item)

        if b.size() == 0:
            del self.bucket_map[val]
            if self.min_val == val:
                self.min_val += 1

        # find bucket+1. Create if needed. Insert item in bucket
        if val+1 in self.bucket_map:
            b = self.bucket_map[val+1]
        else:
            b = Bucket(val+1)
            self.bucket_map[val+1] = b

        b.insert(item)
        self.item_map[item] = b

    def __insert(self, item):
        '''
        new item, insert into Bucket(1)
        '''
        if 1 in self.bucket_map:
            b = self.bucket_map[1]
        else:
            b = Bucket(1)
            self.bucket_map[1] = b
            self.min_val = 1

        b.insert(item)
        self.item_map[item] = b

    def __eject_and_insert(self, item):
        '''
        eject lowest ranked item, insert new item with 
        ejected_value+1
        '''

        b = self.bucket_map[self.min_val]
        old = b.oldest()
        new_val = self.min_val + 1
        b.remove(old)
        del self.item_map[old]
        if b.size() == 0:
            del self.bucket_map[self.min_val]
            self.min_val += 1

        if new_val in self.bucket_map:
            b = self.bucket_map[new_val]
        else:
            b = Bucket(new_val)
            self.bucket_map[new_val] = b

        b.insert(item)
        self.item_map[item] = b

    def add(self, item):
        '''
        adds an item to the summarized stream
        '''
        if item in self.item_map:
            self.__increment(item)
        elif len(self.item_map) < self.size:
            self.__insert(item)
        else:
            self.__eject_and_insert(item)

    def exists(self, item):
        return item in self.item_map


    def clear(self):
        self.bucket_map = {}
        self.item_map = {}
        self.min_val = 0

    def to_list(self):
        return [item for item in self.item_map]

    def save(self):
        bm = self.bucket_map
        data = {bm[item].value(): bm[item].items() for item in bm}
        return {'size': self.size, 'data': data}

    def load(self, data):
        self.clear()
        self.size = data['size']

        buckets = data['data']
        
        for d in buckets:
            if d < self.min_val or self.min_val is 0:
                self.min_val = d
            b = Bucket(d)
            b._items = buckets[d]
            self.bucket_map[d] = b
            for item in buckets[d]:
                self.item_map[item] = b
            
            
