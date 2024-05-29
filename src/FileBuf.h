#ifndef FILEBUF_H
#define FILEBUF_H 1

#include <stddef.h>
#include <stdint.h>

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

#endif /* FILEBUF_H */
