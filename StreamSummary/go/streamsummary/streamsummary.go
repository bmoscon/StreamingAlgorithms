/*******************************************************************************************
 Copyright (C) 2013-2017 Bryant Moscon - bmoscon@gmail.com

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
************************************************************************************/

package streamsummary

import (
	"container/list"
	"fmt"
)

type bucket struct {
	entries list.List
	value   uint64
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

type StreamSummary struct {
	size          int
	bucketMap     map[string]*bucket
	valueMap      map[uint64]*bucket
	smallestValue uint64
}

func NewStreamSummary(capacity int) *StreamSummary {
	s := new(StreamSummary)
	s.size = capacity
	s.bucketMap = make(map[string]*bucket)
	s.valueMap = make(map[uint64]*bucket)

	return s
}

func (s *StreamSummary) Print() {
	for _, bucket := range s.valueMap {
		bucket.print()
	}
}

func (s *StreamSummary) Exists(item string) bool {
	_, found := s.bucketMap[item]
	return found
}

func (s *StreamSummary) GetStream() []string {
	ret := make([]string, len(s.bucketMap))

	i := 0
	for key := range s.bucketMap {
		ret[i] = key
		i++
	}

	return ret
}

func (s *StreamSummary) Add(item string) {
	// does item already exist?
	b, found := s.bucketMap[item]

	if found {
		// if so, find old bucket, and remove item
		oldValue := b.getValue()
		b.remove(item)
		if b.getSize() == 0 {
			// if old bucket is now empty, remove it
			delete(s.valueMap, oldValue)

			// update smallest value if necessary
			if oldValue == s.smallestValue {
				s.smallestValue++
			}
		}

		// see if a bucket exists for item's new value
		newb, found := s.valueMap[oldValue+1]
		if found {
			// it does, so insert it
			newb.insert(item)
			s.bucketMap[item] = newb
		} else {
			// it doesnt, so we'll have to add a new bucket
			newbucket := newBucket(oldValue + 1)
			newbucket.insert(item)
			s.bucketMap[item] = newbucket
			s.valueMap[oldValue+1] = newbucket
		}
	} else if len(s.bucketMap) < s.size {
		// are we within allowable capacity?

		s.smallestValue = 1

		// does bucket 1 exist?
		b1, found := s.valueMap[1]
		if found {
			b1.insert(item)
			s.bucketMap[item] = b1
		} else {
			newbucket := newBucket(1)
			newbucket.insert(item)
			s.bucketMap[item] = newbucket
			s.valueMap[1] = newbucket
		}
	} else {
		// item didnt already exist, and we are over the size
		// so we need to evict smallest item,
		// and use its value + 1 as item to insert's value
		evictBucket := s.valueMap[s.smallestValue]
		evictValue := evictBucket.getValue()
		evictItem := evictBucket.getMin()
		evictBucket.remove(evictItem)

		// remove entry in bucket_map
		delete(s.bucketMap, evictItem)

		// if bucket empty, cleanup
		if evictBucket.getSize() == 0 {
			delete(s.valueMap, evictValue)

			// update smallest value if necessary
			if evictValue == s.smallestValue {
				s.smallestValue++
			}
		}

		newbucket, found := s.valueMap[evictValue+1]
		if found {
			newbucket.insert(item)
			s.bucketMap[item] = newbucket
		} else {
			b := newBucket(evictValue + 1)
			b.insert(item)
			s.valueMap[evictValue+1] = b
			s.bucketMap[item] = b
		}
	}
}
