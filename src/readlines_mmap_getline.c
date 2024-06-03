#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include "cread/src/readlines_mmap_getline.h"

#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cread/src/common.h"
#include "cread/src/FileBuf.h"
#include "cread/src/readlines_getline.h"

bool readlines_mmap_getline(FILE *stream, FileBuf fbuf[static 1])
{
    const int fd = fileno(stream);
    struct stat st;

    if (fstat(fd, &st) == -1) {
        close(fd);
        return false;
    }

    char *const map =
        mmap(nullptr, (size_t) st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (map == MAP_FAILED) {
        return false;
    }

    posix_madvise(map, (size_t) st.st_size, POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED);
    
    FILE *const memstream = fmemopen(map, (size_t) st.st_size, "r");

    if (memstream == nullptr) {
        munmap(map, (size_t) st.st_size);
        return false;
    }

    const bool result = readlines_getline(memstream, fbuf);

    if (!result) {
        munmap(map, (size_t) st.st_size);
        fclose(memstream);
        return false;
    }

    /* Ignore errors on read-only file. */
    munmap(map, (size_t) st.st_size);
    fclose(memstream);

    /* Trim to maximum used. */
    fbuf->lines = safe_trim(fbuf->lines, fbuf->count * sizeof fbuf->lines[0]);
    return true;
}

void readlines_mmap_getline_cleanup(FileBuf fbuf[static 1])
{
    for (size_t i = 0; i < fbuf->count; ++i) {
        free(fbuf->lines[i].line);
    }
    free(fbuf->lines);
}

