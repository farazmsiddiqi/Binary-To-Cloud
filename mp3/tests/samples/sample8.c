#include "tester-utils.h"

int main() {
    // Test if realloc with 0 returns NULL ptr;
    void *ptr1 = malloc(1000);
    void *ptr1_new = realloc(ptr1,0);
    if(ptr1_new != NULL)
      return 1;

    // If I am reducing size, it should use the same block?
    void *ptr2 = malloc(1000);
    void *ptr3 = realloc(ptr2, 500);
    void *ptr4 = realloc(ptr3, 1000);
    void *ptr5 = realloc(ptr4, 500);
    void *ptr6 = realloc(ptr5, 1000);

    return 0;
}
