from ..stream_summary import Bucket, StreamSummary
import random


'''
Simple trivial tests for Bucket simply to ensure that
if the Bucket functionality/design changes that
the unit tests catch it
'''

def test_bucket_insertion():
    b = Bucket(1)
    assert(b.size() == 0)

    b.insert('a')
    assert(b.size() == 1)

    b.remove('a')
    assert(b.size() == 0)


def test_bucket_oldest():
    b = Bucket(5)
    assert(b.value() == 5)

    for i in range(0, 2048):
        b.insert(i)
        assert(b.size() == i+1)

    assert(b.oldest() == 0)

    for i in range(0, 2048):
        b.remove(i)
        assert(b.size() == 2048 - (i+1))
        if b.size():
            assert(b.oldest() == i+1)
    assert(b.size() == 0)



def test_stream_summary_add():
    '''
    Test that adding up to size N results in the object being 
    added to bucket 1, and that no other buckets are created, and that the 'age' order
    in the bucket is correct
    '''
    ss = StreamSummary(2)
    ss.add(1)
    ss.add(2)
    assert(ss.bucket_map[1].oldest() == 1)
    ss.add(3)
    assert(ss.exists(1) == False)
    assert(ss.exists(2) == ss.exists(3) == True)

    assert(1 in ss.bucket_map)
    assert(2 in ss.bucket_map)
    assert(ss.bucket_map[1].size() == 1)
    assert(ss.bucket_map[2].size() == 1)


def test_stream_summary_add_complex():
    ss = StreamSummary(5)
    for i in range(5):
        ss.add(i+1)

    ss.add(1)
    ss.add(1)
    ss.add(10)

    assert(ss.bucket_map[3].items[0] == 1)
    assert(ss.bucket_map[2].items[0] == 10)
    assert(ss.bucket_map[1].size() == 3)
    assert(ss.bucket_map[1].oldest() == 3)


def test_stress_test():
    ss = StreamSummary(1000)

    for i in range(5000000):
        ss.add(random.randint(0, 2500))

    list_from_buckets = []
    for b in ss.bucket_map.keys():
        list_from_buckets.extend(ss.bucket_map[b].items)

    list_from_map = ss.to_list()
    assert(len(list_from_map) == len(list_from_buckets))
    assert(set(list_from_map) == set(list_from_buckets))

    

    
    
