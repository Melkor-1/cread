#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include "cread/src/readlines_getline.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>

#include "cread/src/common.h"
#include "cread/src/FileBuf.h"

bool readlines_getline(FILE *stream, FileBuf fbuf[static 1])
{
    posix_fadvise(fileno(stream), 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED);

    char *line = {};
    size_t capacity = {};

    for (;;) {
        ssize_t nread = getline(&line, &capacity, stream);

        if (nread == -1) {
            if (feof(stream)) {
                /* Input ended due to end-of-file. */
                break;
            }

            goto cleanup_and_fail;
        }

        fbuf->size += (size_t) nread;

        if (nread > 0 && line[nread - 1] == '\n') {
            line[--nread] = '\0';
        }

        if ((fbuf->capacity <= fbuf->count) && !resize_fbuf(fbuf)) {
            goto cleanup_and_fail;
        }

        /* Let getline() allocate another buffer. This saves us from making
         * a copy as well as an allocation. */
        if (!append_line(fbuf, (size_t) nread, line)) {
            goto cleanup_and_fail;
        }
        line = nullptr;
        capacity = 0;
    }

    free(line);

    /* Trim to maximum used, */
    fbuf->lines = safe_trim(fbuf->lines, fbuf->count * sizeof fbuf->lines[0]);
    return true;

  cleanup_and_fail:
    free(line);
    readlines_getline_cleanup(fbuf);
    return false;
}

void readlines_getline_cleanup(FileBuf fbuf[static 1])
{
    for (size_t i = 0; i < fbuf->count; ++i) {
        free(fbuf->lines[i].line);
    }
    free(fbuf->lines);
}

