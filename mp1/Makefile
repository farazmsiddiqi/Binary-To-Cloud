# Select version of g++
ifneq (, $(shell which g++-9))
CXX = g++-9 -g $(CS240)
else
ifneq (, $(shell which g++-11))
CXX = g++-11 -g $(CS240)
else
ifneq (, $(shell which g++))
CXX = g++ -g $(CS240)
else
$(error No g++ compiler found.)
endif
endif
endif


CC = ${CXX} -x c

CFLAGS_CATCH = -fpermissive -w
CFLAGS = -W -Wall -Wno-pointer-sign

main: emoji.o emoji-translate.o main.o
	${CXX} $^ -o $@

all: main test

main.o: main.c
	$(CC) $(CFLAGS) $^ -c -o $@

emoji.o: emoji.c
	$(CC) $(CFLAGS) $^ -c -o $@

emoji-translate.o: emoji-translate.c
	$(CC) $(CFLAGS) $^ -c -o $@


test: emoji.o emoji-translate.o tests/test-emoji.o tests/test-translation.o tests/test.o
	$(CXX)  $^ -o $@

tests/test.o: tests/test.cpp
	$(CXX) $^ -c -o $@

tests/test-emoji.o: tests/test-emoji.cpp
	$(CXX) $(CFLAGS_CATCH) -w $^ -c -o $@

tests/test-translation.o: tests/test-translation.cpp
	$(CXX) $(CFLAGS_CATCH) -w $^ -c -o $@


clean:
	rm -f main test *.o tests/*.o