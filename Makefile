CPP=g++ -std=c++11 -pthread -Wall -O3 -ffast-math -funroll-loops
EXEFILES=run-tests timing-thread-example

all: $(EXEFILES)


####################################################################################################


lexical_cast.o: lexical_cast.cpp lexical_cast.hpp
	$(CPP) -c $<

timing_thread.o: timing_thread.cpp timing_thread.hpp
	$(CPP) -c $<

run-tests.o: run-tests.cpp lexical_cast.hpp arithmetic_inlines.hpp
	$(CPP) -c $<

timing-thread-example.o: timing-thread-example.cpp timing_thread.hpp
	$(CPP) -c $<


####################################################################################################


run-tests: run-tests.o lexical_cast.o
	$(CPP) -o run-tests $^

timing-thread-example: timing-thread-example.o timing_thread.o
	$(CPP) -o timing-thread-example $^

clean:
	rm -f *~ *.o EXEFILES
