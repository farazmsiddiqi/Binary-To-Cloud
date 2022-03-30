#include <cstring>

#include "lib/catch.hpp"
#include "../lib/png.h"

TEST_CASE("`PNG_open` fails on an invalid filename", "[weight=1][part=1][suite=start]") {
  REQUIRE( PNG_open("this-is-not-a-valid-filename", "r") == NULL );
}

TEST_CASE("`PNG_open` validates a valid PNG signature", "[weight=1][part=1][suite=start]") {
  system("cp tests/files/240.png TEST_240.png");
  PNG *png;
  REQUIRE( (png = PNG_open("TEST_240.png", "r")) != NULL );
  PNG_close(png);
  system("rm -f TEST_240.png");
}

TEST_CASE("`PNG_open` fails a non-PNG signature file", "[weight=1][part=1][suite=start]") {
  system("cp tests/files/test_hide.gif TEST_hide.gif");
  REQUIRE( PNG_open("TEST_hide.gif", "r") == NULL );
  system("rm -f TEST_hide.gif");
}

TEST_CASE("`PNG_read` returns PNG file chunks correctly", "[weight=5][part=1]") {
  system("cp tests/files/240.png TEST_240.png");

  PNG *png;

  png = PNG_open("TEST_240.png", "r");

  PNG_Chunk chunk;

  PNG_read(png, &chunk);
  CHECK(strcmp(chunk.type, "IHDR") == 0);
  CHECK(chunk.len == 13);
  PNG_free_chunk(&chunk);

  PNG_read(png, &chunk);
  CHECK(strcmp(chunk.type, "sRGB") == 0);
  CHECK(chunk.len == 1);
  PNG_free_chunk(&chunk);

  PNG_read(png, &chunk);
  CHECK(strcmp(chunk.type, "gAMA") == 0);
  CHECK(chunk.len == 4);
  PNG_free_chunk(&chunk);

  PNG_read(png, &chunk);
  CHECK(strcmp(chunk.type, "pHYs") == 0);
  CHECK(chunk.len == 9);
  PNG_free_chunk(&chunk);

  PNG_read(png, &chunk);
  CHECK(strcmp(chunk.type, "IDAT") == 0);
  CHECK(chunk.len == 2351);
  PNG_free_chunk(&chunk);

  PNG_read(png, &chunk);
  CHECK(strcmp(chunk.type, "IEND") == 0);
  CHECK(chunk.len == 0);
  PNG_free_chunk(&chunk);

  PNG_close(png);
  system("rm -f TEST_240.png");
}

TEST_CASE("`PNG_read` return values correct", "[weight=1][part=1]") {
  system("cp tests/files/240.png TEST_240.png");

  PNG *png;

  png = PNG_open("TEST_240.png", "r");

  PNG_Chunk chunk;

  CHECK(PNG_read(png, &chunk) == 25);   // IHDR
  PNG_free_chunk(&chunk);
  CHECK(PNG_read(png, &chunk) == 13);   // sRGB
  PNG_free_chunk(&chunk);
  CHECK(PNG_read(png, &chunk) == 16);   // gAMA
  PNG_free_chunk(&chunk);
  CHECK(PNG_read(png, &chunk) == 21);   // pHYs
  PNG_free_chunk(&chunk);
  CHECK(PNG_read(png, &chunk) == 2363); // IDAT
  PNG_free_chunk(&chunk);
  CHECK(PNG_read(png, &chunk) == 12);   // IEND
  PNG_free_chunk(&chunk);

  PNG_close(png);
  system("rm -f TEST_240.png");
}

TEST_CASE("`PNG_open` writes a PNG header", "[weight=1][part=1]") {
  PNG *png;
  png = PNG_open("TEST_output.png", "w");
  REQUIRE(png != NULL);
  PNG_close(png);

  FILE *f = fopen("TEST_output.png", "r");
  REQUIRE(f != NULL);

  unsigned char *buffer = (unsigned char *) malloc(8);
  fread(buffer, sizeof(char), 8, f);
  fclose(f);

  CHECK( buffer[0] == 0x89 );
  CHECK( buffer[1] == 0x50 );
  CHECK( buffer[2] == 0x4e );
  CHECK( buffer[3] == 0x47 );
  CHECK( buffer[4] == 0x0d );
  CHECK( buffer[5] == 0x0a );
  CHECK( buffer[6] == 0x1a );
  CHECK( buffer[7] == 0x0a );

  free(buffer);
  system("rm -f TEST_output.png");
}


TEST_CASE("`PNG_write` writes a PNG chunk and CRC", "[weight=1][part=1]") {
  PNG *png;

  // Write TEST_output.png
  png = PNG_open("TEST_output.png", "w");

  PNG_Chunk chunk;
  chunk.type[0] = 'I';
  chunk.type[1] = 'H';
  chunk.type[2] = 'D';
  chunk.type[3] = 'R';
  chunk.type[4] = 0x00;
  chunk.len = 4;
  chunk.data = (unsigned char *)"uiuc";
  PNG_write(png, &chunk);

  PNG_close(png);

  // Verify contents...
  FILE *f = fopen("TEST_output.png", "r");
  REQUIRE(f != NULL);

  unsigned char *buffer = (unsigned char *) malloc(24);
  fread(buffer, sizeof(char), 24, f);
  fclose(f);

  // Length (in network byte order):
  CHECK( buffer[ 8] == 0 );
  CHECK( buffer[ 9] == 0 );
  CHECK( buffer[10] == 0 );
  CHECK( buffer[11] == 4 );

  // Chunk Type:
  CHECK( buffer[12] == 'I' );
  CHECK( buffer[13] == 'H' );
  CHECK( buffer[14] == 'D' );
  CHECK( buffer[15] == 'R' );

  // Chunk Data:
  CHECK( buffer[16] == 'u' );
  CHECK( buffer[17] == 'i' );
  CHECK( buffer[18] == 'u' );
  CHECK( buffer[19] == 'c' );

  // CRC (in network byte order):
  CHECK( buffer[20] == 0x6f );
  CHECK( buffer[21] == 0x1a );
  CHECK( buffer[22] == 0xf4 );
  CHECK( buffer[23] == 0x30 );

  free(buffer);
  system("rm -f TEST_output.png");
}

TEST_CASE("`PNG_write` writes a PNG chunk with correct endianness", "[weight=1][part=1]") {
  PNG *png;

  // Write TEST_output.png
  png = PNG_open("TEST_output.png", "w");

  PNG_Chunk wchunk;
  wchunk.type[0] = 'I';
  wchunk.type[1] = 'H';
  wchunk.type[2] = 'D';
  wchunk.type[3] = 'R';
  wchunk.type[4] = 0x00;
  wchunk.len = 4;
  wchunk.data = (unsigned char *)"uiuc";
  PNG_write(png, &wchunk);

  PNG_close(png);

  // Verify contents...
  png = PNG_open("TEST_output.png", "r");

  PNG_Chunk rchunk;

  CHECK(PNG_read(png, &rchunk) == 16);
  CHECK(strcmp(rchunk.type, "IHDR") == 0);
  // Length (in little endian order):
  CHECK(rchunk.len == 4);
  // CRC (in little endian order):
  CHECK(rchunk.crc == 0x6f1af430);

  PNG_free_chunk(&rchunk);
  PNG_close(png);

  system("rm -f TEST_output.png");
}
