CXX = g++
CXXFLAGS = -g -std=c++14 -Wall -Wextra -pedantic

.PHONY: solution.zip

all: csim

csim: main.o cache.o
	$(CXX) $(CXXFLAGS) -o csim main.o cache.o

main.o: main.cpp cache.h
	$(CXX) $(CXXFLAGS) -c main.cpp

cache.o: cache.cpp cache.h
	$(CXX) $(CXXFLAGS) -c cache.cpp

solution.zip :
	rm -f $@
	zip -9r $@ *.cpp *.h Makefile README.txt

clean:
	rm -f *.o csim