#include <stdlib.h>

int main() {
  const int TEST_SIZE = 10240;
  char *a = malloc(TEST_SIZE);

  int i;
  for (i = 0; i < TEST_SIZE; i++) {
    *a = 'a';
  }

  free(a);
  return 0;
}
