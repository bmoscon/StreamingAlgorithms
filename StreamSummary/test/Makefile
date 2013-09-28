# Makefile for StreamSummary test program
# 
# March 2013 - Bryant Moscon


ss_test: ss_test.o
	g++ -o ss_test -g -Wall -Wextra -Wshadow -pedantic ss_test.o -std=c++11

ss_test.o: ss_test.cc ../stream_summary.hpp
	g++ -c -g -Wall -Wextra -Wshadow -pedantic ss_test.cc -std=c++11

clean:
	rm ss_test ss_test.o
