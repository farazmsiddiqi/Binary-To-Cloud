#include <stdio.h>
#include <stdlib.h>
#include "lib/png.h"
#include <string.h>

int png_hideGIF(const char *png_filename, const char *gif_filename) {
  PNG *png = PNG_open(png_filename, "r+");
  if (!png) { return ERROR_INVALID_FILE; }
  PNG_Chunk uiuc_chunk;
  //PNG_Chunk end_chunk;

  // open file
  FILE* gif = fopen(gif_filename, "r");
  // get len of file, populate chunk len
  fseek(gif, 0L, SEEK_END);
  int len = ftell(gif);
  fseek(gif, 0L, SEEK_SET);
  uiuc_chunk.len = len;
  // populate chunk type
  uiuc_chunk.type[0] = 'u';
  uiuc_chunk.type[1] = 'i';
  uiuc_chunk.type[2] = 'u';
  uiuc_chunk.type[3] = 'c';
  uiuc_chunk.type[4] = 0x0;
  // populate chunk data
  uiuc_chunk.data = malloc(uiuc_chunk.len);
  fread(uiuc_chunk.data, 1, uiuc_chunk.len, gif);
  // 0 initialize the chunk crc
  uiuc_chunk.crc = 0;
  // close file
  fclose(gif);

  while (1) {
    // Read chunk and ensure we get a valid result (exit on error):
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      return ERROR_INVALID_CHUNK_DATA;
    }

    /*
    1. currently, file pointer is at the end of the IEND chunk
    2. move the fp num_bytes_in_chunk backwards (potentially 12);
    3. one fp is 12 bytes backward, overwrite IEND chunk with uiuc_chunk
    4. after uiuc_chunk written, then write IEND chunk
    5. free memory.
    */

   if ( strcmp(chunk.type, "IEND") == 0 ) {

     fseek(png->fp, -12L, SEEK_CUR);

    //  // capture IEND chunk
    //  end_chunk.len = chunk.len;

    //  end_chunk.type[0] = chunk.type[0];
    //  end_chunk.type[1] = chunk.type[1];
    //  end_chunk.type[2] = chunk.type[2];
    //  end_chunk.type[3] = chunk.type[3];
    //  end_chunk.type[4] = chunk.type[4];

    //  end_chunk.data = malloc(chunk.len);
    //  strncat(end_chunk.data, chunk.data, chunk.len);
    //  //end_chunk.data = chunk.data; // do i malloc?
    //  end_chunk.crc = chunk.crc;

     // overwrite IEND chunk
     PNG_write(png, &uiuc_chunk);

     // add IEND chunk after uiuc_chunk
     PNG_write(png, &chunk);

     //PNG_free_chunk(&end_chunk);
     PNG_free_chunk(&uiuc_chunk);
     PNG_free_chunk(&chunk);
     break;
    }

    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }

  PNG_close(png);
  return 0; 
}

int main(int argc, char *argv[]) {
  // Ensure the correct number of arguments:
  if (argc != 3) {
    printf("Usage: %s <PNG File> <GIF File>\n", argv[0]);
    return ERROR_INVALID_PARAMS;
  }

  return png_hideGIF(argv[1], argv[2]);
}
