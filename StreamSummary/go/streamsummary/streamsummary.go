/******************************************************************************************* 
 Copyright (C) 2013-2015 Bryant Moscon - bmoscon@gmail.com
 
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
    place and form as other copyright, license and disclaimer information.

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
************************************************************************************/

package streamsummary

import (
	"fmt"
	"container/list"
)

type bucket struct {
	entries  list.List
        value    uint64
}

func newBucket(value uint64) *bucket {
	b := new(bucket)
	b.value = value

	return b
}

func (b *bucket) print() {
	fmt.Print(b.value, ": ")
	i := b.entries.Front()
	for {
		if i == nil {
			fmt.Println(" ")
			return
		}
		
		fmt.Print(i.Value.(string), " ")
		
		i = i.Next()
	}
}

func (b *bucket) insert(s string) {
	b.entries.PushBack(s)
}

func (b *bucket) getValue() uint64 {
	return b.value
}

func (b *bucket) getSize() int {
	return b.entries.Len()
}

func (b *bucket) getMin() string {
	return (b.entries.Front().Value.(string))
}

func (b *bucket) remove(s string) {
	i := b.entries.Front()
	for {
		if i == nil {
			panic("tried to remove a non-existent entry")
		}
		
		if i.Value.(string) == s {
			b.entries.Remove(i)
			return
		}
		
		i = i.Next()
	}
}

type streamSummary struct {
	size            int
	bucket_map      map[string]*bucket
	value_map       map[uint64]*bucket
	smallest_value  uint64
}

func NewStreamSummary(capacity int) *streamSummary {
	s := new(streamSummary)
	s.size = capacity
	s.bucket_map = make(map[string]*bucket)
	s.value_map = make(map[uint64]*bucket)

	return s
}

func (s *streamSummary) Print() {
	for _, bucket := range s.value_map {
		bucket.print()
		
	}
}

func (s *streamSummary) Exists(item string) bool {
	_, found := s.bucket_map[item]
	return found
}

func (s *streamSummary) GetStream() []string {
	ret := make([]string, len(s.bucket_map))
	
	i := 0
	for key, _ := range s.bucket_map {
		ret[i] = key
		i += 1
	}

	return ret
}

func (s *streamSummary) Add(item string) {
	// does item already exist?
	b, found := s.bucket_map[item]
	
	if found {
		// if so, find old bucket, and remove item
		old_value := b.getValue()
		b.remove(item)
		if b.getSize() == 0 {
			// if old bucket is now empty, remove it
			delete(s.value_map, old_value)

			// update smallest value if necessary
			if old_value == s.smallest_value {
				s.smallest_value = s.smallest_value + 1
			}
		}

		// see if a bucket exists for item's new value
		new_b, found := s.value_map[old_value + 1]
		if found {
			// it does, so insert it
			new_b.insert(item)
			s.bucket_map[item] = new_b
		} else {
			// it doesnt, so we'll have to add a new bucket
			new_bucket := newBucket(old_value + 1)
			new_bucket.insert(item)
			s.bucket_map[item] = new_bucket
			s.value_map[old_value + 1] = new_bucket
		}
		
	// are we within allowable capacity?
	} else if len(s.bucket_map) < s.size {

		s.smallest_value = 1

		// does bucket 1 exist?
		b1, found := s.value_map[1]
		if found {
			b1.insert(item)
			s.bucket_map[item] = b1
		} else {
			new_bucket := newBucket(1)
			new_bucket.insert(item)
			s.bucket_map[item] = new_bucket
			s.value_map[1] = new_bucket
		}
	} else {
		// item didnt already exist, and we are over the size
		// so we need to evict smallest item,
		// and use its value + 1 as item to insert's value
		evict_bucket := s.value_map[s.smallest_value]
		evict_value := evict_bucket.getValue()
		evict_item := evict_bucket.getMin()
		evict_bucket.remove(evict_item)

		// remove entry in bucket_map
		delete(s.bucket_map, evict_item)

		// if bucket empty, cleanup
		if evict_bucket.getSize() == 0 {
			delete(s.value_map, evict_value)
			
			// update smallest value if necessary
			if evict_value == s.smallest_value {
				s.smallest_value = s.smallest_value + 1
			}
		}

		new_bucket, found := s.value_map[evict_value + 1]
		if found {
			new_bucket.insert(item)
			s.bucket_map[item] = new_bucket
		} else {
			b := newBucket(evict_value + 1)
			b.insert(item)
			s.value_map[evict_value + 1] = b
			s.bucket_map[item] = b
		}
	}
}
