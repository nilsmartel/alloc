#include "alloc.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Size of the heap (in bytes).
#define SIZE (1024 * 1024 * 1)


/// Heap-memory area.
char memory[SIZE] = {0};


/// Memory-chunk structure.
struct mblock {
  struct mblock *next;
  size_t size;
  char memory[];
};

/// Pointer to the first element of the free-memory list.
static struct mblock *head = (struct mblock *)memory;

size_t bytes_after_block(struct mblock *node) {
  if (node->next == NULL)
    return SIZE - ((size_t)node->memory - (size_t)memory);

  return (size_t)node->next - (size_t)node->memory;
}

void *malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  // Actual amount of bytes requires mblock to be aligned with memory
  size_t required_size = size + sizeof(struct mblock);

  struct mblock *current = head;

  while (bytes_after_block(current) < required_size) {
    if (current->next == NULL) {
      printf("No memory available");
      errno = ENOMEM;
      return NULL;
    }

    current = current->next;
  }

  if (current->next != NULL) {
    struct mblock *link = current->next;
    current->next = (struct mblock *)((size_t)current->memory + current->size);
    struct mblock *node = current->next;

    node->next = link;
    node->size = size;
    return node->memory;
  }

  current->next = (struct mblock *)((size_t)current->memory + current->size);
  struct mblock *node = current->next;
  node->next = NULL;
  node->size = size;

  return node->memory;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    struct mblock* last = head;
    struct mblock* current = head;

    while (ptr != current->memory) {
        // the current mblock is _not_ the one we want to free

        if (current->next == NULL) {
            // No follow up mblock, the pointer is invalid
            abort();
        }
        last = current;
        current = current->next;
    }

    // we now have the mblock to be freed in `current`
    last->next = current->next;

    if (last->size==0) last->next = NULL;
}
