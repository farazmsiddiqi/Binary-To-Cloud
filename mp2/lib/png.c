#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "crc32.h"
#include "png.h"

const int ERROR_INVALID_PARAMS = 1;
const int ERROR_INVALID_FILE = 2;
const int ERROR_INVALID_CHUNK_DATA = 3;
const int ERROR_NO_UIUC_CHUNK = 4;


/**
 * Opens a PNG file for reading (mode == "r" or mode == "r+") or writing (mode == "w").
 * 
 * (Note: The function follows the same function prototype as `fopen`.)
 * 
 * When the file is opened for reading this function must verify the PNG signature.  When opened for
 * writing, the file should write the PNG signature.
 * 
 * This function must return NULL on any errors; otherwise, return a new PNG struct for use
 * with further functions in this library.
 */
PNG * PNG_open(const char *filename, const char *mode) {
  FILE* fp = fopen(filename, mode);
  if (fp == NULL) return NULL;

  if (*mode == 'w' || *mode == 'r+') {
    unsigned char* buffer = malloc(9);
    buffer[0] = 0x89;
    buffer[1] = 0x50;
    buffer[2] = 0x4e;
    buffer[3] = 0x47;
    buffer[4] = 0x0d;
    buffer[5] = 0x0a;
    buffer[6] = 0x1a;
    buffer[7] = 0x0a;
    buffer[8] = 0x0;

    fwrite(buffer, sizeof(char), sizeof(buffer), fp);
    struct _PNG* png = malloc(100000);
    png->fp = fp;
    free(buffer);
    return png;
  }

  char* fp_signature = malloc(9);
  fread(fp_signature, sizeof(char), 8, fp);
  fp_signature[8] = 0x0;

  if (fp_signature[0] == '\x89' && fp_signature[1] == '\x50'
      && fp_signature[2] == '\x4E' && fp_signature[3] == '\x47'
      && fp_signature[4] == '\xD' && fp_signature[5] == '\xA'
      && fp_signature[6] == '\x1A' && fp_signature[7] == '\xA') {
    
    struct _PNG* png = malloc(10000);
    free(fp_signature);
    png->fp = fp;
    return png;
  } else {
    free(fp_signature);
    return NULL;
  }
}


/**
 * Reads the next PNG chunk from `png`.
 * 
 * If a chunk exists, the data in the chunk is populated in `chunk` and the
 * number of bytes read (the length of the chunk in the file) is returned.
 * Otherwise, a zero value is returned.
 * 
 * Any memory allocated within `chunk` must be freed in `PNG_free_chunk`.
 * Users of the library must call `PNG_free_chunk` on all returned chunks.
 */
size_t PNG_read(PNG *png, PNG_Chunk *chunk) {

  if (png->fp == NULL) return 0;

  size_t chunk_full_len = 0;
  
  // Read len var into chunk

  fread(&chunk->len, sizeof(uint32_t), 1, png->fp);
  chunk->len = ntohl(chunk->len);
  chunk_full_len += sizeof(uint32_t);
  
  // Read type str into chunk
  fread(&chunk->type, sizeof(char), 4, png->fp);
  chunk->type[4] = '\0';
  chunk_full_len += 4*sizeof(char);

  // Read data into chunk
  if (chunk->len != 0) {

    chunk->data = malloc(chunk->len + 1);
    chunk->data[chunk->len] = '\0';
    for (uint32_t i = 0; i < chunk->len; i++) {
      fread(&chunk->data[i], sizeof(unsigned char), 1, png->fp);
    }
    chunk_full_len += chunk->len;

  } else {
    chunk->data = NULL;
  }

  // Read crc var into chunk
  fread(&chunk->crc, sizeof(uint32_t), 1, png->fp);
  chunk->crc = ntohl(chunk->crc);
  chunk_full_len += sizeof(uint32_t);

  return chunk_full_len;
}


/**
 * Writes a PNG chunk to `png`.
 * 
 * Returns the number of bytes written.
 */
size_t PNG_write(PNG *png, PNG_Chunk *chunk) {

  size_t chunk_full_len = 0;
  
  // Write len var from chunk
  // translate to network endianness, write, and decode back to host endianness
  uint32_t network_len = htonl(chunk->len);
  fwrite(&network_len, sizeof(uint32_t), 1, png->fp);
  chunk_full_len += sizeof(uint32_t);
  
  // Write type str from chunk
  fwrite(&chunk->type, sizeof(char), 4, png->fp);
  chunk_full_len += 4*sizeof(char);

  // Write data from chunk
  if (chunk->len != 0) {
    fwrite(chunk->data, sizeof(unsigned char), chunk->len, png->fp);
    chunk_full_len += chunk->len;
  }

  // Write crc var into chunk from type and data
  char* type_and_data = malloc(4 + chunk->len);
  for (uint32_t i = 0; i < 4; i++) {
    type_and_data[i] = chunk->type[i];
  }
  for (uint32_t i = 0; i < chunk->len; i++) {
    type_and_data[i+4] = chunk->data[i];
  }
  
  uint32_t calcd_crc = 0;

  crc32(type_and_data, chunk->len + 4, &calcd_crc); // in host byte format
  uint32_t network_crc = htonl(calcd_crc);
  fwrite(&network_crc, sizeof(uint32_t), 1, png->fp);
  free(type_and_data);
  chunk_full_len += sizeof(uint32_t);

  return chunk_full_len;
}


/**
 * Frees all memory allocated by this library related to `chunk`.
 */
void PNG_free_chunk(PNG_Chunk *chunk) {
  if (chunk->len != 0) {
    free(chunk->data);
    chunk->data = NULL;
  }
}


/**
 * Closes the PNG file and frees all memory related to `png`.
 */
void PNG_close(PNG *png) {
  fclose(png->fp);
  free(png);
  png = NULL;
}