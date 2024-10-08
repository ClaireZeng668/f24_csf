CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic

all: csim

csim: main.o cache.o
	$(CXX) $(CXXFLAGS) -o csim main.o cache.o

main.o: main.cpp cache.h
	$(CXX) $(CXXFLAGS) -c main.cpp

cache.o: cache.cpp cache.h
	$(CXX) $(CXXFLAGS) -c cache.cpp

clean:
	rm -f *.o csim