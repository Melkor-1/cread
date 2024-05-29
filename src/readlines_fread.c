#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include "readlines_fread.h"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>

#include "common.h"
#include "FileBuf.h"

/* Reads the next chunk of data from the stream referenced to by `stream`.
 * `chunk` must be a pointer to an array of size `size`. 
 *
 * Returns the number of bytes read on success, or zero elsewise. The chunk is 
 * not null-terminated.
 *
 * `read_next_chunk()` does not distinguish between end-of-file and error; the
 * routines `feof()` and `ferror()` must be used to determine which occured. */
[[nodiscard, gnu::nonnull]] static size_t read_next_chunk(FILE *restrict stream, 
                                                          size_t size,
                                                          char chunk[restrict static size]) 
{
    const size_t rcount = fread(chunk, 1, size, stream);

    if (rcount < size) {
        if (!feof(stream)) {
            /* A read error occured. */
            return 0;
        }

        if (rcount == 0) {
            return 0;
        }
    }
    
    return rcount;
}

bool readlines_fread(FILE *stream, FileBuf fbuf[static 1])
{
#define MIN_CHUNK_SIZE 65536u

#if   SIZE_MAX >= (MIN_CHUNK_SIZE * 4 - 1)
    #define CHUNK_SIZE (MIN_CHUNK_SIZE * 4 - 1)
#elif SIZE_MAX >= (MIN_CHUNK_SIZE * 2 - 1)
    #define CHUNK_SIZE (MIN_CHUNK_SIZE * 2 - 1)
#elif SIZE_MAX >= (MIN_CHUNK_SIZE  - 1)         /* Required by ISO C. */
    #define CHUNK_SIZE (MIN_CHUNK_SIZE - 1)
#else
    #error "The compiler is non-conforming to ISO C."
#endif
    posix_fadvise(fileno(stream), 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED);

    char chunk[CHUNK_SIZE];
    size_t nread; 
    size_t remain = 0;

    while (nread = read_next_chunk(stream, CHUNK_SIZE - remain, chunk + remain)) {
        fbuf->size += nread;
        nread += remain;    /* Include remaining bytes from the previous read. */
        remain = 0;         /* Reset remain for the current iteration. */

        for (char *lhs = chunk, *rhs; lhs < &chunk[nread]; lhs = rhs + 1) {
            rhs = memchr(lhs, '\n', (size_t) (&chunk[nread] - lhs));

            if (rhs == nullptr) {
                /* We did not find a newline. We shalt consider this an
                 * incomplete line and copy the bytes after the last '\n'
                 * to the beginning of the chunk. */
                remain = (size_t) (&chunk[nread] - lhs);
                memmove(chunk, chunk + (lhs - chunk), remain);
                break;
            }

            if ((fbuf->capacity <= fbuf->count) && !resize_fbuf(fbuf)) {
                goto cleanup_and_fail;
            }

            if (!alloc_and_append_line(fbuf, (size_t) (rhs - lhs), lhs)) {
                goto cleanup_and_fail; 
            }
        }
    }
    
    if (remain) {
        if ((fbuf->capacity <= fbuf->count) && !resize_fbuf(fbuf)) {
            goto cleanup_and_fail;
        }

        if (!alloc_and_append_line(fbuf, remain, chunk)) {
            goto cleanup_and_fail; 
        }
    }

    /* Trim to maximum used. */
    fbuf->lines = safe_trim(fbuf->lines, fbuf->count * sizeof fbuf->lines[0]);
    return feof(stream);

  cleanup_and_fail:
    readlines_fread_cleanup(fbuf);
    return false;
}

void readlines_fread_cleanup(FileBuf fbuf[static 1])
{
    for (size_t i = 0; i < fbuf->count; ++i) {
        free(fbuf->lines[i].line);
    }
    free(fbuf->lines);
}
