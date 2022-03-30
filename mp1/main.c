#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"

int main() {

  char * test = malloc(10);
  strcpy(test, "\xF0\x9F\x8E\x89");



  return 0;
}