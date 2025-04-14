CXX = g++
CXXFLAGS = -std=c++17

.PHONY: all clean

all: run

run: main.o segment.o
	$(CXX) $(CXXFLAGS) -o $@ main.o segment.o

%.o: %.cpp ktree.h segment.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f run *.o
