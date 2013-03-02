# Makefile for StreamSummary test program
# 
# March 2013 - Bryant Moscon


ss_test: ss_test.o
	g++ -o ss_test -g -Wall ss_test.o

ss_test.o: ss_test.cc ../stream_summary.hpp
	g++ -c -g -Wall ss_test.cc

clean:
	rm ss_test ss_test.o
