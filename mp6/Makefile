##ifdef SOLUTION
#ifndef MP2_DIR
#MP2_DIR = ../../mp2/solution/
#endif
##endif

FLAGS = -g -Wall -DSOLUTION
CC = gcc

ifndef MP2_DIR
MP2_DIR = ../mp2/
endif

LIBS = $(MP2_DIR)lib/crc32.c $(MP2_DIR)lib/png.c

all: png-extractGIF

png-extractGIF: $(MP2_DIR)png-extractGIF.c $(LIBS)
	$(CC) $^ $(FLAGS) -o $@

clean:
	rm -rf *.dSYM/ png-extractGIF
