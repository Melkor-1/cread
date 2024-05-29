#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include "readlines_read.h"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "FileBuf.h"

/* Reads as many bytes as possible, up to `buf_size` bytes, from file descriptor
 * `fd` into buffer `buf`. 
 *
 * Returns the number of bytes read (zero indicates end of file), or -1 on
 * failure. */
[[nodiscard]] static ssize_t read_all(int fd, 
                                      size_t buf_size, 
                                      char buf[static buf_size])
{
    size_t total_read = {};
    ssize_t nread;

    while ((nread = read(fd, buf + total_read, buf_size - total_read)) > 0) {
        total_read += (size_t) nread;
    }

    return nread < 0 ? nread : (ssize_t) total_read;
}


/* Reads the next chunk of data from the file descriptor `fd`.
 * `chunk` must be a pointer to an array of size `size`. 
 *
 * On success, returns the number of bytes read (zero indicates end of file).
 * On error, -1 is returned and `errno` is set appropriately by `read()`.
 *
 * The chunk is not null-terminated. */
[[nodiscard, gnu::always_inline]] static inline ssize_t read_next_chunk(
        int fd, 
        size_t size, 
        char chunk[static size]) 
{
    return read_all(fd, size, chunk);
}

bool readlines_read(FILE *stream, FileBuf fbuf[static 1])
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
    /* Fail early if `stream` is not associated with a file, as that is the only
     * error case specified from `fileno()`. If we can't get a file descriptor
     * from `stream`, we can't read from it with `read()`. */
    const int fd = fileno(stream);    

    if (fd == -1) {
        return false;
    }

    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED);

    char chunk[CHUNK_SIZE];
    ssize_t remain = {};

    while (true) {
        ssize_t nread = read_next_chunk(fd, (size_t) (CHUNK_SIZE - remain), 
                                        chunk + remain);

        if (nread == 0) {
            /* end of file */
            break;
        }

        if (nread == -1) {
            goto cleanup_and_fail;
        }

        fbuf->size += (size_t) nread;
        nread += remain;    /* Include remaining bytes from the previous read. */
        remain = 0;         /* Reset remain for the current iteration. */

        for (char *lhs = chunk, *rhs; lhs < &chunk[nread]; lhs = rhs + 1) {
            rhs = memchr(lhs, '\n', (size_t) (&chunk[nread] - lhs));

            if (rhs == nullptr) {
                /* We did not find a newline. We shalt consider this an
                 * incomplete line and copy the bytes after the last '\n'
                 * to the beginning of the chunk. */
                remain = &chunk[nread] - lhs;
                memmove(chunk, chunk + (lhs - chunk), (size_t) remain);
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

        if (!alloc_and_append_line(fbuf, (size_t) remain, chunk)) {
            goto cleanup_and_fail; 
        }
    }

    /* Trim to maximum used. */
    fbuf->lines = safe_trim(fbuf->lines, fbuf->count * sizeof fbuf->lines[0]);
    return true;

  cleanup_and_fail:
    readlines_read_cleanup(fbuf);
    return false;
}

void readlines_read_cleanup(FileBuf fbuf[static 1])
{
    for (size_t i = 0; i < fbuf->count; ++i) {
        free(fbuf->lines[i].line);
    }
    free(fbuf->lines);
}
