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

CXX += -g $(CS240)
CFLAGS = -W -Wall -Wno-pointer-sign
CFLAGS_CATCH = -fpermissive -w
CC = ${CXX} -x c ${CFLAGS}


all: png-analyze png-extractGIF png-hideGIF png-rewrite

# .o rules:
crc.o: lib/crc32.c
	${CC} $^ -c -o $@

png.o: lib/png.c
	${CC} $^ -c -o $@

png-analyze.o: png-analyze.c
	${CC} $^ -c -o $@

png-extractGIF.o: png-extractGIF.c
	${CC} $^ -c -o $@

png-hideGIF.o: png-hideGIF.c
	${CC} $^ -c -o $@

png-rewrite.o: png-rewrite.c
	${CC} $^ -c -o $@


# exe rules:
png-analyze: crc.o png.o png-analyze.o
	${CXX} $^ -o $@

png-extractGIF: crc.o png.o png-extractGIF.o
	${CXX} $^ -o $@

png-hideGIF: crc.o png.o png-hideGIF.o
	${CXX} $^ -o $@

png-rewrite: crc.o png.o png-rewrite.o
	${CXX} $^ -o $@


# tests
test: png.o crc.o tests/test-extract.cpp tests/test-hide.cpp tests/test-libpng.cpp tests/test.o
	$(CXX) $(CFLAGS_CATCH) $^ -o $@

tests/test.o: tests/test.cpp
	$(CXX) $(CFLAGS_CATCH) $^ -c -o $@



clean:
	rm -f png-analyze png-extractGIF png-hideGIF png-rewrite test *.o TEST_* TEST.gif TEST.png
