/**
 * Malloc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _metadata_t {
  unsigned int size;     // The size of the memory block.
  unsigned char isUsed;  // 0 if the block is free; 1 if the block is used.
  void * next;
  void * prev;
} metadata_t;

void *startOfHeap = NULL;
void *free_head = NULL;
void *free_tail = NULL;

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 * @see https://danluu.com/malloc-tutorial/
 */
void *calloc(size_t num, size_t size) {
  // no allocation of memory
  void* request = malloc(num*size);

  // set mem field bytes to 0x0
  memset(request, 0x0, num*size);

  if (request == (void*) -1) {
    return NULL;
  }

  return request;
}

/**
 * Adds chunk to the free list
 * 
 * @param meta pointer to newly split block's metadata
 * 
 * @return void
 */
void free_list_add(metadata_t *meta) {
  if (free_head == NULL && free_tail == NULL) {
    free_head = free_tail = meta;
    meta->prev = NULL;
    meta->next = NULL;
  }

  else if ((void *)meta > free_tail && free_tail != NULL) {
    ((metadata_t *)free_tail)->next = meta;
    meta->prev = free_tail;
    meta->next = NULL;
    free_tail = (void *)meta;
  }

  else if ((void *)meta < free_head) {
    meta->next = free_head;
    meta->prev = NULL;
    ((metadata_t *)free_head)->prev = meta;
    free_head = (void *)meta;
  }
  else { // to ensure that free list is listed in order of memory address
    metadata_t *probe = free_head;
    metadata_t *cur = free_head;
    probe = cur->next; 

    while (probe != NULL && (void *)probe < (void *)meta) { // while probe has a lower memory address than meta
      cur = cur->next;
      probe = probe->next;
    }
    metadata_t *tmp = cur->next;
    cur->next = meta;
    meta->next = tmp;
    meta->prev = cur;
    if (probe != NULL) probe->prev = meta;
  }
}


/**
 * Removes newly allocated block from the free list
 * 
 * @param curMeta Pointer to the current memory block that is now having memory allocated into it
 * 
 * @return void
*/
void free_list_remove(metadata_t * meta) {
  if (meta == free_head) {
    if (meta->next == NULL) free_head = NULL;
    else free_head = meta->next;
  }
  if (meta == free_tail) {
    metadata_t *tmp = free_tail;
    free_tail = ((metadata_t *)free_tail)->prev;
    tmp->prev = NULL;
    tmp->next = NULL;
  }

  if (meta->prev != NULL) ((metadata_t *)meta->prev)->next = meta->next;
  if (meta->next != NULL) ((metadata_t *)meta->next)->prev = meta->prev;
  meta->prev = NULL;
  meta->next = NULL;
}

void coalesce_block(metadata_t *curMeta) {
  metadata_t * coal_itr = (void *)curMeta;
  size_t size_tracker = coal_itr->size; // initialize size to current meta chunk's size
  // if the memory is contiguous on the heap and thus coalescable, continue to check for unused blocks
  if ((void *)coal_itr + coal_itr->size + sizeof(metadata_t) == coal_itr->next ) {
    coal_itr = coal_itr->next;
  } else return;

  while (coal_itr != NULL) {
    size_tracker += coal_itr->size + sizeof(metadata_t);
    metadata_t * tmp = coal_itr;
    curMeta->size = size_tracker; // coalesce regardless of whether size is big enough or not (similar to path compression in 225)

    // if the memory is contiguous on the heap and thus coalescable, continue to check for unused blocks
    if ((void *)coal_itr + coal_itr->size + sizeof(metadata_t) == coal_itr->next ) {
      coal_itr = coal_itr->next;
      free_list_remove(tmp);
    } else {
      free_list_remove(tmp);
      return;
    }
  }
}

void * split_block(metadata_t * curMeta, size_t size) {
  unsigned int og_size = curMeta->size;
  metadata_t * og_meta = curMeta;

  curMeta->isUsed = 1;
  curMeta->size = size;
  metadata_t * return_ptr = (void *)curMeta + sizeof(metadata_t);
  
  curMeta = (void *)curMeta + sizeof(metadata_t) + size; // increment pointer to the location where the new metadata chunk should reside
  metadata_t * split_ptr = (void *)curMeta;
  split_ptr->isUsed = 0;
  split_ptr->size = og_size - sizeof(metadata_t) - size; // make size equal the remaining memory after SPLIT
  free_list_add(split_ptr);

  free_list_remove(og_meta); 

  return (void *)return_ptr; // return a pointer to the beginning of the allocated memory block
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void * malloc(size_t size) {
  if (size == 0) return NULL; 
  if (startOfHeap == NULL) startOfHeap = sbrk(0); // If we have not recorded the start of the heap, record it:

  // initialize to the beginning of the free linked list, all blocks iterated through are free 
  metadata_t * curMeta = free_head;
  void * endOfHeap = sbrk(0); // returns the current end of heap without increasing it

  while ((void *)curMeta < endOfHeap && curMeta != NULL) {   // While we're before the end of the heap...
    
    // If a block has enough memory to hold the new memory allocation + another memory chunk & metadata
    if (curMeta->size > size + sizeof(metadata_t)) return split_block(curMeta, size); // SPLIT
    // if a block is currently unused and size fits w/ no room remaining for a new chunk
    if (curMeta->size >= size) { // NON-SPLIT
      curMeta->isUsed = 1;
      free_list_remove(curMeta); // remove block from the free linked list
      return (void *)curMeta + sizeof(metadata_t);
    }

    // If no split and no fit... check COALESCE
    coalesce_block(curMeta); 
    if (curMeta->size >= size) continue; // if there is now enough room, run the loop over
    
    // no split, no fit, no coalesce means move the pointer onto the next potential location
    // curMeta = (void *)curMeta + sizeof(metadata_t) + curMeta->size; // ORIGINAL, NO FREE LIST
    curMeta = curMeta->next; // FREE LIST Implementation
  }
  // if while loop exits, then there were no valid holes to fill, coalesce or split -> move on to allocate new memory

  metadata_t *meta = sbrk( sizeof(metadata_t) ); // Allocate heap memory for the metadata structure and populate the metadata variables
  if (meta == (void *)-1) return NULL; // if not allocated correctly, return NULL
  // else
  meta->size = size;
  meta->isUsed = 1;
  meta->next = NULL;
  meta->prev = NULL;

  void *return_ptr = sbrk( size ); // Allocate heap memory for the requested memory:

  // if the function fails to allocate the requested block of memory, return null pointer
  if (return_ptr == (void *)-1) {
    meta->size = 0;
    free(meta); // free the already allocated meta;
    return NULL;
  }

  // Return the pointer for the requested memory:
  return return_ptr;
}


/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
  if (ptr == NULL) return;
  
  // Find the metadata located immediately before `ptr`:
  metadata_t *meta = ptr - sizeof( metadata_t );
  meta->isUsed = 0; // Mark the allocation as free:

  free_list_add(meta);
  // coalHelper(meta); // 0 for dummy call
}



/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the 'size' param bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 * @see https://danluu.com/malloc-tutorial/ --> this website provided some of the inspiration for realloc and calloc.
 */
void *realloc(void *ptr, size_t size) {
  
  if (ptr == NULL) {
    return malloc(size);
  }

  if (size == 0) {
    free(ptr);
    return NULL;
  }

  metadata_t* meta = ptr - sizeof(metadata_t);

  if (meta->size >= size) {
    // current block can store newly allocated block
    return ptr;
  } else {
    // block needs to be moved to a place with enough space for it to store more data
    void* request = malloc(size);

    if (request == NULL) {
      return NULL;
    }

    memcpy(request, ptr, meta->size);
    free(ptr);
    return request;
  }
}