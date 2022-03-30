#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../emoji.h"
#include "lib/catch.hpp"


/**
 * Returns `true` if string `s` starts with an emoji.
 * 
 * This is a crude function that returns `false` when the string `s` absolutely does not start
 * with an emoji.  When this function returns `true`, `s` starts with something that is within
 * the range of valid emoji characters.  However, this range is sparse and this function does
 * not check if the emoji is a defined emoji.
 * 
 * See Unicode's Emoji List: https://www.unicode.org/Public/UCD/latest/ucd/emoji/emoji-data.txt
 */
int isEmoji(const char *s) {
  unsigned int val = 0;
  for(int i=0; i<strlen(s); i++) {
    val = (val << 8) | ((unsigned int)(s[i]) & 0xFF);
  }

  return (
    (val >= 14844092 /* U+203C */ && val <= 14912153 /* U+3299 */) ||
    (val >= 4036984960 /* U+1F000 */ && val <= 4036996031 /* U+1FAFF */ )
  );
}


TEST_CASE("`emoji_favorite` returns a valid emoji", "[weight=1][part=1][suite=start]") {
  const char *s = emoji_favorite();
  REQUIRE(strcmp(s, "") != 0);
  REQUIRE(isEmoji(s) != 0);
}


TEST_CASE("`emoji_count` counts one emoji", "[weight=1][part=1]") {
  char *s = (char *) malloc(100);
  strcpy(s, "hi\xF0\x9F\x8E\x89");
  int r = emoji_count(s);
  REQUIRE(r == 1);
  free(s);
}

TEST_CASE("`emoji_count` counts multiple emoji", "[weight=3][part=1]") {
  char *s = (char *) malloc(100);
  strcpy(s, "\xF0\x9F\x92\x96 \xF0\x9F\x92\xBB \xF0\x9F\x8E\x89");
  int r = emoji_count(s);
  REQUIRE(r == 3);
  free(s);
}

TEST_CASE("`emoji_count` returns zero with no emojis", "[weight=1][part=1]") {
  char *s = (char *) malloc(100);
  strcpy(s, "This string contains no emojis.");
  int r = emoji_count(s);
  REQUIRE(r == 0);
  free(s);
}

TEST_CASE("`emoji_count` correctly detects UTF-8 non-emojis", "[weight=3][part=1]") {
  char *s = (char *) malloc(100);
  strcpy(s, "Not an emoji: \xF0\x98\x92\x96");
  int r = emoji_count(s);
  REQUIRE(r == 0);
  free(s);
}


TEST_CASE("`emoji_invertChar` inverts smiley face into another emoji", "[weight=1][part=1]") {
  char *s = (char *)malloc(100);
  strcpy(s, "\xF0\x9F\x98\x8A");
  emoji_invertChar(s);
  REQUIRE(strcmp(s, "\xF0\x9F\x98\x8A") != 0);
  free(s);
}

TEST_CASE("`emoji_invertChar` inverts at least six total emojis", "[weight=3][part=1]") {
  int emoji_invert_count = 0;

  unsigned int i;
  for (i = 4036984960; i <= 4036995737; i++) {
    char emoji[5];
    /* memcpy() results depends on machine architecture and endian-ness */
    emoji[0] = (i >> 24) & 0xFF;
    emoji[1] = (i >> 16) & 0xFF;
    emoji[2] = (i >> 8) & 0xFF;
    emoji[3] = i & 0xFF;
    emoji[4] = '\0';
    emoji_invertChar(emoji);

    int* val = (int *)emoji;
    if (*val != i) {
      emoji_invert_count++;
      if (emoji_invert_count >= 6) { break; }
    }
  }

  REQUIRE( emoji_invert_count >= 6 );
}

TEST_CASE("`emoji_invertChar` does not invert string with no emoji", "[weight=1][part=1]") {
  char *s = (char *) malloc(100);
  char *s_orig = "string with no emoji";
  strcpy(s, s_orig);
  emoji_invertChar(s);
  REQUIRE(strcmp(s, s_orig) == 0);

  s_orig = "another string with no emoji";
  strcpy(s, s_orig);
  emoji_invertChar(s);
  REQUIRE(strcmp(s, s_orig) == 0);
  free(s);
}


TEST_CASE("`emoji_invertAll` inverts a string of emojis", "[weight=3][part=1]") {
  char *s = (char *) malloc(100);
  strcpy(s, "\xF0\x9F\x92\x96 \xF0\x9F\x92\xBB \xF0\x9F\x8E\x89 \xF0\x9F\x98\x8A");
  emoji_invertAll(s);
  char *testing_emoji = (char *) malloc(20);
  strcpy(testing_emoji, s + 15);
  testing_emoji[4] = '\0';
  REQUIRE(strcmp(testing_emoji, "\xF0\x9F\x98\x8A") != 0);
  free(testing_emoji);
  free(s);
}

TEST_CASE("`emoji_random_alloc` allocates new memory", "[weight=1][part=1]") {
  char *s1 = emoji_random_alloc();
  char *s2 = emoji_random_alloc();
  REQUIRE(s1 != s2);
  free(s2);
  free(s1);
}

TEST_CASE("`emoji_random_alloc` allocates valid emoji", "[weight=3][part=1]") {
  const int total_emojis = 100;
  int valid_emoji = 0;

  for (int i = 0; i < total_emojis; i++) {
    char *s = emoji_random_alloc();
    if (s != NULL && isEmoji(s)) { valid_emoji++; }
    free(s);
  }

  REQUIRE( valid_emoji == total_emojis );
}

int min(int a, int b) {
  if (a < b) { return a; }
  else       { return b; }
}

TEST_CASE("`emoji_random_alloc` allocates random emoji", "[weight=3][part=1]") {
  const int total_emojis = 1000;
  int unique_emoji = 0;

  char **emojis = (char **) malloc(total_emojis * sizeof(char *));
  int i;
  for (i = 0; i < total_emojis; i++) {
    emojis[i] = emoji_random_alloc();
    if (emojis[i] == NULL) { break; }

    int isUnique = 1;
    for (int j = 0; j < i; j++) {
      if (strcmp(emojis[i], emojis[j]) == 0) {
        isUnique = 0; break;
      }
    }

    if (isUnique) {
      unique_emoji++;
    }
  }

  for (int j = 0; j <= min(total_emojis - 1, i); j++) {
    free(emojis[j]);
  }
  free(emojis);

  // There will randomly be some non-unique emoji.  However, in the range of over ~2,816,
  // the chance of having less than 500 unique emoji when randomly drawn is vanishingly small.
  REQUIRE( unique_emoji > total_emojis * 0.5 );
}


TEST_CASE("`emoji_invertFile_alloc` inverts all smiley faces - short text", "[weight=1][part=1]") {
  unsigned char *inverted_content = emoji_invertFile_alloc("tests/txt/invert-short.txt");
  REQUIRE( inverted_content != NULL );

  char *testing_emoji = (char *) malloc(5);
  strncpy(testing_emoji, (char *) inverted_content, 4);
  testing_emoji[4] = '\0';
  REQUIRE(strcmp(testing_emoji, "\xF0\x9F\x98\x8A") != 0);
  
  strncpy(testing_emoji, (char *) inverted_content + 5, 4);
  REQUIRE(strcmp(testing_emoji, "\xF0\x9F\x98\x8A") != 0);

  REQUIRE(strlen((char *) inverted_content) == 9);
  free(testing_emoji);
  free(inverted_content);
}

TEST_CASE("`emoji_invertFile_alloc` inverts all smiley faces - long text", "[weight=1][part=1]") {
  unsigned char *inverted_content = emoji_invertFile_alloc("tests/txt/invert-long.txt");
  REQUIRE( inverted_content != NULL );

  char *testing_emoji = (char *) malloc(5);
  strncpy(testing_emoji, (char *) inverted_content + 17, 4);
  testing_emoji[4] = '\0';
  REQUIRE(strcmp(testing_emoji, "\xF0\x9F\x98\x8A") != 0);

  REQUIRE(strlen((char *) inverted_content) == 19);
  free(testing_emoji);
  free(inverted_content);
}

TEST_CASE("`emoji_invertFile_alloc` invalid file name", "[weight=1][part=1]") {
  unsigned char *inverted_content = emoji_invertFile_alloc("tests/txt/nonexistent.txt");
  REQUIRE(inverted_content == NULL);
}