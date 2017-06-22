CPP=g++ -std=c++11 -pthread -Wall -O3 -ffast-math -funroll-loops

EXEFILES=run-tests argument-parser-example timing-thread-example yaml-paramfile-example

all: $(EXEFILES)


####################################################################################################


argument_parser.o: argument_parser.cpp argument_parser.hpp lexical_cast.hpp
	$(CPP) -c $<

lexical_cast.o: lexical_cast.cpp lexical_cast.hpp
	$(CPP) -c $<

timing_thread.o: timing_thread.cpp timing_thread.hpp
	$(CPP) -c $<

yaml_paramfile.o: yaml_paramfile.cpp yaml_paramfile.hpp
	$(CPP) -c $<

run-tests.o: run-tests.cpp lexical_cast.hpp arithmetic_inlines.hpp
	$(CPP) -c $<

argument-parser-example.o: argument-parser-example.cpp argument_parser.hpp
	$(CPP) -c $<

timing-thread-example.o: timing-thread-example.cpp timing_thread.hpp
	$(CPP) -c $<

yaml-paramfile-example.o: yaml-paramfile-example.cpp yaml_paramfile.hpp
	$(CPP) -c $<


####################################################################################################


run-tests: run-tests.o lexical_cast.o
	$(CPP) -o $@ $^

argument-parser-example: argument-parser-example.o argument_parser.o lexical_cast.o
	$(CPP) -o $@ $^

timing-thread-example: timing-thread-example.o timing_thread.o
	$(CPP) -o $@ $^

yaml-paramfile-example: yaml-paramfile-example.o yaml_paramfile.o
	$(CPP) -o $@ $^ -lyaml-cpp

clean:
	rm -f *~ *.o $(EXEFILES)

