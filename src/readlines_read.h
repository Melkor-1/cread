#ifndef READLINES_READ_H
#define READLINES_READ_H 1

#include <stdio.h>

#include "cread/src/FileBuf.h"

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns `true`. The calling code is required to call 
 * `readlines_read_cleanup()` at some point in the code to release the memory 
 * associated with the lines.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool readlines_read(FILE *stream, 
                                                FileBuf fbuf[static 1]);

void readlines_read_cleanup(FileBuf fbuf[static 1]);

#endif /* READLINES_READ_H */
