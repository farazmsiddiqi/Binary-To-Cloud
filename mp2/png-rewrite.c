#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/png.h"

int png_rewrite(const char *png_filename_in, const char *png_filename_out) {
  // Open the file specified in argv[1] for reading and argv[2] for writing:
  PNG *png = PNG_open(png_filename_in, "r");
  if (!png) { return ERROR_INVALID_FILE; }

  PNG *out = PNG_open(png_filename_out, "w");
  printf("PNG Header written.\n");
  size_t bytesWritten;


  // Read chunks until reaching "IEND" or in invalid chunk:
  while (1) {
    // Read chunk and ensure we get a valid result (exit on error):
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      PNG_close(out);
      return ERROR_INVALID_CHUNK_DATA;
    }

    // Report data about the chunk to the command line:
    bytesWritten = PNG_write(out, &chunk);
    printf("PNG chunk %s written (%lu bytes)\n", chunk.type, bytesWritten);

    // Check for the "IEND" chunk to exit:
    if ( strcmp(chunk.type, "IEND") == 0 ) {
      PNG_free_chunk(&chunk);
      break;  
    }

    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }

  PNG_close(out);
  PNG_close(png);
  return 0;
}


int main(int argc, char *argv[]) {
  // Ensure the correct number of arguments:
  if (argc != 3) {
    printf("Usage: %s <SOURCE PNG> <DEST PNG>\n", argv[0]);
    return ERROR_INVALID_PARAMS;
  }
  
  return png_rewrite(argv[1], argv[2]);
}