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

void panic(const char *msg) {
  fprintf(stderr, "%s", msg);
  exit(0);
}

struct mblock empty_block() {
  struct mblock block = {NULL, 0};
  return block;
}

size_t free_bytes_after_block(struct mblock *node) {
  if (node->next == NULL)
    return SIZE - (size_t)node->memory;

  return (size_t)node->next - (size_t)node->memory;
}

void *malloc(size_t size) {
  if (size == 0)
    panic("required memory of size 0");
  // Actual amount of bytes requires mblock to be aligned with memory
  size_t required_size = size + sizeof(struct mblock);

  struct mblock *current = head;

  while (free_bytes_after_block(current) < required_size) {
    if (current->next == NULL)
      panic("no memory left");

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
  current->size = size;
  current->next = (struct mblock *)(current->memory + size);
  current->next[0] = empty_block();
  return current->memory;
}

void free(void *ptr) {
  if (ptr == head->memory) {
    head->size = 0;
    return;
  }

  struct mblock *current = head;

  while (current->next != NULL) {
    if (current->next->memory == ptr) {
        current->next = current->next->next;
        return;
    }
  }

  panic("attempt to free unallocated memory region");
}

void *realloc(void *ptr, size_t size) {
    struct mblock *current = head;
    while (current->memory != ptr) {
        if (current->next == NULL) panic("tried to reallocate unallocated pointer");
        current = current->next;
    }

    if (free_bytes_after_block(current) <= size) {
        current->size = size;
        return current;
    }

    struct mblock *new = malloc(size);

    memcpy(current->memory, new, current->size);
    free(current->memory);

    return new;
}
