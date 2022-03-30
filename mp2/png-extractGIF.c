#include <stdio.h>
#include <stdlib.h>
#include "lib/png.h"
#include <string.h>
#include <errno.h>

int png_extractGIF(const char *png_filename, const char *gif_filename) {
  PNG *png = PNG_open(png_filename, "r");
  if (!png) { return ERROR_INVALID_FILE; }
  printf("PNG Header: OK\n"); 
  // reversed for an mp6 optimization (hack)
  int hasUiucChunk = 1;

  while (1) {
    // Read chunk and ensure we get a valid result (exit on error):
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      return ERROR_INVALID_CHUNK_DATA; 
    }

    // Report data about the chunk to the command line:
    printf("Chunk: %s (%d bytes of data)\n", chunk.type, chunk.len);


    // Check for the "uiuc" chunk to extract:
    if ( strcmp(chunk.type, "uiuc") == 0 ) {
      hasUiucChunk = 0;
      // open gif file
      FILE* gif = fopen(gif_filename, "w");
      fwrite(chunk.data, sizeof(unsigned char), chunk.len, gif);
      fclose(gif);
    }

    if ( strcmp(chunk.type, "IEND") == 0 ) {
      PNG_free_chunk(&chunk);
      break;
    }

    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }

  PNG_close(png); 
  return hasUiucChunk; // 0 on success, 1 on event:no_uiuc_chunk
}


int main(int argc, char *argv[]) {
  // Ensure the correct number of arguments:
  if (argc != 3) {
    printf("Usage: %s <PNG File> <GIF Name>\n", argv[0]);
    return ERROR_INVALID_PARAMS;
  }
  return png_extractGIF(argv[1], argv[2]);
}
