#ifndef ALLOC_H
#define ALLOC_H

#include <sys/types.h>

/*
   malloc() allocates size bytes and returns a pointer to the
   allocated memory. The memory is not cleared.

   RETURN VALUE: The value returned is a pointer
   to the allocated memory or NULL if the request fails. The
   errno will be set to indicate the error.
*/
void *malloc(size_t size);

/*
   free() frees the memory space pointed to by ptr, which must
   have been returned by a previous call to malloc(). Otherwise,
   or if free(ptr) has already been called before the program is
   aborted. If ptr is NULL, no operation is performed.

   RETURN VALUE: no value
*/
void free(void *ptr);

void *realloc(void *ptr, size_t size);



#endif
