# Select version of g++
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

CC = gcc
WARNINGS = -Wall -Wno-unused-result -Wno-unused-function -Wno-unused-parameter 
CFLAGS_COMMON = $(WARNINGS) -std=c99 -D_GNU_SOURCE -lm
CFLAGS_RELEASE = $(CFLAGS_COMMON) -O3
CFLAGS_CATCH = $(CFLAGS_COMMON) -w -fpermissive

INC = -I.
FLAGS = -W -Wall -g -fPIC $(CS240)
LINKOPTS = -pthread

all: degree hedgehog-simple ping-pong gacha

wallet.o: lib/wallet.c
	$(CC) -Ilib $(FLAGS) -c lib/wallet.c -o wallet.o


degree.o: degree.c
	$(CC) $(INC) $(FLAGS) -c $^ -o $@

degree: degree.o wallet.o
	$(CC) $(INC) $(FLAGS) $(LINKOPTS) $^ -o $@


hedgehog-simple.o: hedgehog-simple.c
	$(CC) $(INC) $(FLAGS) -c $^ -o $@

hedgehog-simple: hedgehog-simple.o wallet.o
	$(CC) $(INC) $(FLAGS) $(LINKOPTS) $^ -o $@


ping-pong.o: ping-pong.c
	$(CC) $(INC) $(FLAGS) -c $^ -o $@

ping-pong: ping-pong.o wallet.o
	$(CC) $(INC) $(FLAGS) $(LINKOPTS) $^ -o $@


gacha.o: gacha.c
	$(CC) $(INC) $(FLAGS) -c $^ -o $@

gacha: gacha.o wallet.o
	$(CC) $(INC) $(FLAGS) $(LINKOPTS) $^ -o $@


.PHONY: clean
clean:
	rm -rf *.dSYM/ wallet.o degree.o degree test tests/test.o hedgehog-simple.o hedgehog-simple *.o gacha ping-pong

tests/test.o: tests/test.cpp
	$(CXX) $(CFLAGS_CATCH) $^ -c -o $@

test: wallet.o tests/test.o tests/test-wallet.cpp tests/test-hedgehog.cpp tests/test-degree.cpp tests/test-ping-pong.cpp
	$(CXX) $(CFLAGS_CATCH) $(LINKOPTS) $^ -o $@
