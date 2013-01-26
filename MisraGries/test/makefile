# Makefile for Misra-Gries test program
# 
# Jan 2013 - Bryant Moscon


misra_gries_test: misra_gries_test.o
	g++ -o misra_gries_test misra_gries_test.o

misra_gries.o: misra_gries_test.cpp ../misra_gries.hpp
	g++ -c misra_gries_test.cpp

clean:
	rm misra_gries_test misra_gries_test.o
