# Select version of g++:
ifneq (, $(shell which g++-9))
CXX = g++-9
else
ifneq (, $(shell which g++-11))
CXX = g++-11
else
ifneq (, $(shell which g++))
CXX = g++
else
$(error No g++ compiler found.)
endif
endif
endif

CC = $(CXX) -x c -g -Wall 
CFLAGS_CATCH = -fpermissive -w $(CS240) -pthread -g
CFLAGS = -W -Wall $(CS240) -pthread

all: server

server: http.o server.o
	$(CXX) $(CFLAGS) $^ -o $@

http.o: http.c
	$(CC) $(CFLAGS) $^ -c -o $@

server.o: server.c
	$(CC) $(CFLAGS) $^ -c -o $@

## Test Suite
tests/test.o: tests/test.cpp 
	$(CXX) $(CFLAGS_CATCH) $^ -c -o $@

tests/test-part1.o: tests/test-part1.cpp
	$(CXX) $(CFLAGS_CATCH) $^ -c -o $@

tests/test-part2.o: tests/test-part2.cpp
	$(CXX) $(CFLAGS_CATCH) $^ -c -o $@

tests/test-part3.o: tests/test-part3.cpp
	$(CXX) $(CFLAGS_CATCH) $^ -c -o $@

test: tests/test.o tests/test-part1.o tests/test-part2.o tests/test-part3.o http.o 
	$(CXX) $(CFLAGS_CATCH) $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o tests/*.o server test
