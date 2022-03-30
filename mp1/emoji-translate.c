#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"


void emoji_init(emoji_t *emoji) {
  emoji->emojis = malloc(1000);
  emoji->translations = malloc(1000);
  emoji->idx = 0;
}

/*
Pair a source with a translation of that source using parallel indexing.

@param emoji the object to update with a new source-translation
@param source the emoji (key)
@param translation the meaning of the emoji (value)
*/
void emoji_add_translation(emoji_t *emoji, 
                          const unsigned char *source, 
                          const unsigned char *translation) {

  emoji->emojis[emoji->idx] = malloc(1000);
  strcpy(emoji->emojis[emoji->idx], source);

  emoji->translations[emoji->idx] = malloc(1000);
  strcpy(emoji->translations[emoji->idx], translation);
  
  emoji->idx += 1;
}

// Translates the emojis contained in the file `fileName`.
const unsigned char *emoji_translate_file_alloc(emoji_t *emoji, const char *fileName) {
  /* READ THE FILE */
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


  char * translated_string = calloc(1, 1000);
  // iterate byte by byte
  for (int i = 0; i < (int) strlen(bigass_string); i++) {
    int max_seq = 0;
    int idx_found = -1;

    // iterate through emoji patterns
    for (int j = 0; j < emoji->idx; j++) {
      // this is the len we iterate for 
      int cur_seq_len = strlen(emoji->emojis[j]);

      if (cur_seq_len > (int) strlen(&bigass_string[i])) {
        continue;
      }

      if (strncmp(&bigass_string[i], emoji->emojis[j], cur_seq_len) == 0) {
        if (cur_seq_len > max_seq) {
          max_seq = cur_seq_len;
          idx_found = j;
        }
      }
    }

    if (idx_found != -1) {
      strcat(translated_string, emoji->translations[idx_found]);
      i += strlen(emoji->emojis[idx_found]) - 1;
    } else {
      strncat(translated_string, &bigass_string[i], 1);
    }
  }
  free(bigass_string);
  return translated_string;
}

void emoji_destroy(emoji_t *emoji) {
  for (int i = 0; i < emoji->idx; i++) {
    free(emoji->emojis[i]);
    emoji->emojis[i] = NULL;
    free(emoji->translations[i]);
    emoji->translations[i] = NULL;
  }
  free(emoji->emojis);
  emoji->emojis = NULL;
  free(emoji->translations);
  emoji->translations = NULL;
}
