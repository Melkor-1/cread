#ifndef READLINES_MMAP_MEMCHR
#define READLINES_MMAP_MEMCHR 1

#include <stdio.h>

#include "common.h"

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns `true`. The calling code is required to call 
 * `readlines_mmap_memchr_cleanup()` at some point in the code to release the 
 * memory associated with the lines.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool readlines_mmap_memchr(FILE *stream, 
                                                       FileBuf fbuf[static 1]);

void readlines_mmap_memchr_cleanup(FileBuf fbuf[static 1]);

#endif  /* READLINES_MMAP_MEMCHR */
