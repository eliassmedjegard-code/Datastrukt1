# Makefile för labb 1 - Grafer
# Bygger programmet "labb1" från reader, graph och main.

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

OBJS = main.o reader.o graph.o

labb1: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

main.o: main.cpp reader.h graph.h
	$(CXX) $(CXXFLAGS) -c main.cpp

reader.o: reader.cpp reader.h
	$(CXX) $(CXXFLAGS) -c reader.cpp

graph.o: graph.cpp graph.h reader.h
	$(CXX) $(CXXFLAGS) -c graph.cpp

run: labb1
	./labb1

clean:
	rm -f *.o labb1

.PHONY: run clean
