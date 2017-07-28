all: demo

demo: bloom_example.cpp bloom.hpp counting_bloom.hpp spectral_bloom.hpp ../../hash/MurmurHash3.cpp
	g++ -o demo ../../hash/MurmurHash3.cpp bloom_example.cpp -I../../hash

clean:
	rm -f demo