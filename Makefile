all: lexical_cast.o timing_thread.o

CPP = g++ -std=c++11 -pthread -Wall -O3 -ffast-math -funroll-loops

%.o: %.cpp
	$(CPP) -c $<

