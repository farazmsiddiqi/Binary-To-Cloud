#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Return your favorite emoji.  Do not allocate new memory.
// (This should **really** be your favorite emoji, we plan to use this later in the semester. :))
char *emoji_favorite() {
  // Face with one eyebrow raised: 🤨
  return "🤨";
}

// Count the number of emoji in the UTF-8 string `utf8str`, returning the count.  You should
// consider everything in the ranges starting from (and including) U+1F000 up to (and including) U+1FAFF.
int emoji_count(const unsigned char *utf8str) {
  // if emoji is between \u1F000 and \u1FAFF inclusive, it is an emoji
  if (strlen(utf8str) < 4) {
    return 0;
  }

  int num_emojis = 0;

  size_t left = 0;

  while (left+3 < strlen(utf8str)) {
    char * potential_emoji = malloc(5);
    // strcpy(potential_emoji, utf8str[left] + utf8str[left+1] + utf8str[left+2] + utf8str[left+3]); //bit shift instead?

    for (int i = 0; i < 4; ++i) {
      //strcat(potential_emoji, utf8str[left + i]);
      potential_emoji[i] = utf8str[left + i];
    }

    potential_emoji[4] = '\0';


    if (strcmp(potential_emoji, "\U0001F000") >= 0
        && strcmp(potential_emoji, "\U0001FAFF") <= 0) { // go thru 4 bits at a time
      num_emojis += 1;
    }

    left += 1;
    free(potential_emoji);
    potential_emoji = NULL;
  }
  
  return num_emojis;
}

// Return a random emoji stored in new heap memory you have allocated.  Make sure what
// you return is a valid C-string that contains only one random emoji.

/*
\u1F000
 = 0001 1111 0000 0000 0000
 = 1111 0000 1001 1111 1000 0000 1000 0000

*******\u1F000 = \xF0 \x9F \x80 \x80


\u1FAFF
= 0001 1111 1010 1111 1111
= 1111 0000 1001 1111 1010 1011 1011 1111

*******\u1FAFF = \xF0 \x9F \xAB \xBF

randomize last two bits, keep first two bits the same.
*/
char *emoji_random_alloc() {

  //randomly generate bottom two bits. Range: \x80 \x80 to \xAB \xBF
  //int fixed_bits = 0xF09F; // "\xF0\x9F"

  int b3_upper = 0xAB; // 1010 1011
  int b3_lower = 0x80;
  int b3_rand = (rand() % (b3_upper - b3_lower + 1)) + b3_lower;

  int b4_upper = 0xBF; // 1011 1111
  int b4_lower = 0x80;
  int b4_rand = (rand() % (b4_upper - b4_lower + 1)) + b4_lower;

  char * test = malloc(5);
  test[0] = 0xF0;
  test[1] = 0x9F;
  test[2] = b3_rand;
  test[3] = b4_rand;
  test[4] = 0x0;

  return test;
}


// Modify the UTF-8 string `utf8str` to invert the FIRST character (which may be up to 4 bytes)
// in the string if it the first character is an emoji.  At a minimum:
// - Invert "😊" U+1F60A ("\xF0\x9F\x98\x8A") into ANY non-smiling face.
// - Choose at least five more emoji to invert.

/*

1) 1111 0000 1001 1111 1001 1000 1000 1010 -> \xF0\x9F\x98\x8A
2) 1111 0000 1001 1111 1010 1010 1010 1110 -> \xF0\x9F\xAA\xAE
3) 1111 0000 1001 1111 1010 1010 1010 1100 -> \xF0\x9F\xAA\xAB
4) 1111 0000 1001 1111 1010 1010 1010 0100 -> \xF0\x9F\xAA\xA4
5) 1111 0000 1001 1111 1010 1010 1010 0010 -> \xF0\x9F\xAA\xA2
6) 1111 0000 1001 1111 1010 1010 1010 0001 -> \xF0\x9F\xAA\xA1

*/

void emoji_invertChar(unsigned char *utf8str) {

  if (strlen(utf8str) < 4) {
    printf("%s", "string doesn't have enough bytes to hold an emoji");
  }

  // 1111 0000 1001 1111 1010 1000 1011 0000
  // 0xF09FA8B0
  // create an emoji to replace existing emojis with
  char * replacement_emoji = malloc(5);
  replacement_emoji[0] = 0xF0;
  replacement_emoji[1] = 0x9F;
  replacement_emoji[2] = 0xA8;
  replacement_emoji[3] = 0xB0;
  replacement_emoji[4] = 0x0;

  // pot emoji is a sliced COPY of utf8str
  char * potential_emoji = malloc(5);
  for (int i = 0; i < 4; ++i) {
    potential_emoji[i] = utf8str[i];
  }
  potential_emoji[4] = '\0';

  // if it is an an emoji
  if (strcmp(potential_emoji, "\U0001F000") >= 0
      && strcmp(potential_emoji, "\U0001FAFF") <= 0) {

      // at this point, pot_emoji contains an emoji
      if (strcmp(potential_emoji, "\xF0\x9F\x98\x8A") == 0
          || strcmp(potential_emoji, "\xF0\x9F\xAA\xAE") == 0
          || strcmp(potential_emoji, "\xF0\x9F\xAA\xAB") == 0
          || strcmp(potential_emoji, "\xF0\x9F\xAA\xA4") == 0
          || strcmp(potential_emoji, "\xF0\x9F\xAA\xA2") == 0
          || strcmp(potential_emoji, "\xF0\x9F\xAA\xA1") == 0) {

        // once emoji found, replace orig string with another emoji
        for (size_t i = 0; i < 4; i++) {
          utf8str[i] = replacement_emoji[i];
        }
      }

  }

  free(replacement_emoji);
  replacement_emoji = NULL;
  free(potential_emoji);
  potential_emoji = NULL;
}


// Modify the UTF-8 string `utf8str` to invert ALL of the character by calling your
// `emoji_invertChar` function on each character.
void emoji_invertAll(unsigned char *utf8str) {

  size_t left = 0;
  while (left+3 < strlen(utf8str)) {
    char * potential_emoji = malloc(5);
    for (int i = 0; i < 4; ++i) {
      potential_emoji[i] = utf8str[left + i];
    }
    potential_emoji[4] = '\0';

    emoji_invertChar(potential_emoji);

    for (int i = 0; i < 4; ++i) {
      utf8str[left + i] = potential_emoji[i];
    }

    left += 1;

    free(potential_emoji);
    potential_emoji = NULL;
  }
}


// Reads the full contents of the file `fileName, inverts all emojis, and
// returns a newly allocated string with the inverted file's content.
unsigned char *emoji_invertFile_alloc(const char *fileName) {
  /*
  1. read file contents into a bigass string
  2. call invertAll on the bigass string
  */
  FILE * fp = fopen(fileName, "r");

  if (fp == NULL) {
    return NULL;
  }

  fseek(fp, 0L, SEEK_END);
  size_t len = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  char * bigass_string = malloc(len + 1);
  bigass_string[len] = '\0';

  fread(bigass_string, 1, len, fp);
  fclose(fp);

  emoji_invertAll(bigass_string);


  return bigass_string;
}
