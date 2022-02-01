CXX = g++
CPPFLAGS = -Wall -g 

all: bin/.dirstamp doublet

heap: bin/doublet.o
	$(CXX) $(CPPFLAGS) $^ -o $@

bin/MinHeap.o: doublet.cpp MinHeap.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

bin/.dirstamp:
	mkdir -p bin
	touch bin/.dirstamp