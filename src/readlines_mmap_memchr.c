#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include "readlines_mmap_memchr.h"

#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "FileBuf.h"

bool readlines_mmap_memchr(FILE *stream, FileBuf fbuf[static 1])
{
    const int fd = fileno(stream);
    struct stat st;

    if (fstat(fd, &st) == -1) {
        close(fd);
        return false;
    }

    fbuf->size = (size_t) st.st_size;

    char *const map =
        mmap(nullptr, (size_t) st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    if (map == MAP_FAILED) {
        return false;
    }

    posix_madvise(map, (size_t) st.st_size, POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED);

    for (char *lhs = map, *rhs; lhs < &map[st.st_size]; lhs = rhs + 1) {
        rhs = memchr(lhs, '\n', (size_t) (&map[st.st_size] - lhs));

        if (rhs == nullptr) {
            /* We have reached end-of-file or the file is malformed. */
            break;
        }

        if ((fbuf->capacity <= fbuf->count) && !resize_fbuf(fbuf)) {
            goto cleanup_and_fail;
        }

        if (!append_line(fbuf, (size_t) (rhs - lhs), lhs)) {
            goto cleanup_and_fail; 
        }
    }

    /* Trim to maximum used. */
    fbuf->lines = safe_trim(fbuf->lines, fbuf->count * sizeof fbuf->lines[0]);
    return true;

  cleanup_and_fail:
    munmap(map, fbuf->size);
    free(fbuf->lines);
    return false;
}

void readlines_mmap_memchr_cleanup(FileBuf fbuf[static 1])
{
    munmap(fbuf->lines[0].line, fbuf->size);
    free(fbuf->lines);
}
