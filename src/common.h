#ifndef COMMON_H
#define COMMON_H 1

#include <stdint.h>
#include <stdlib.h>

typedef struct line {
    char *line;
    size_t size;
} Line;

typedef struct file_buf {
    Line *lines;
    size_t capacity;
    size_t count;
    uintmax_t size;
} FileBuf;

[[nodiscard, gnu::always_inline, gnu::returns_nonnull, gnu::nonnull]] static
inline void *safe_trim(void *p, size_t n)
{
    void *const cp = realloc(p, n);

    return cp ? cp : p;
}

[[nodiscard]] bool resize_fbuf(FileBuf fbuf[static 1]);

[[nodiscard]] bool append_line(FileBuf fbuf[static 1], 
                               size_t len,
                               char line[static len]);

[[nodiscard]] bool alloc_and_append_line(FileBuf fbuf[static 1], 
                                         size_t len,
                                         char line[static len]);
#endif /* COMMON_H */
