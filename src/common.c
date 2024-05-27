#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/* If `cap` is less than 8, bumps it up to 8 and returns `true`.
 * Else it stores the result of `cap * 2` in `result`. 
 *
 * The function returns `false` if the multiplication overflowed, or `true`
 * elsewise. result is not modified in case of an overflow. */ 
[[gnu::always_inline]] static inline bool grow_capacity(size_t result[static 1], 
                                                        size_t cap)
{
    if (cap != 0 && 2 > SIZE_MAX / cap) {
        return false;
    }

    *result = cap < 8 ? 8 : cap * 2;
    return true; 
}

bool resize_fbuf(FileBuf fbuf[static 1])
{
    size_t new_capacity = {};
    
    if (!grow_capacity(&new_capacity, fbuf->capacity)) {
        return false;
    }

    void *const tmp = realloc(fbuf->lines, new_capacity * sizeof fbuf->lines[0]);

    if (tmp != nullptr) {
        fbuf->lines = tmp;
        fbuf->capacity = new_capacity;
        return true;
    }

    return false;
}

bool append_line(FileBuf fbuf[static 1], size_t len, char line[static len])
{
    fbuf->lines[fbuf->count].line = line;
    fbuf->lines[fbuf->count].line[len] = '\0';
    fbuf->lines[fbuf->count].size = len + 1;
    return fbuf->count + 1 < fbuf->count ? false : ++fbuf->count;
}

[[nodiscard]] bool alloc_and_append_line(FileBuf fbuf[static 1], 
                                         size_t len,
                                         char line[static len])
{
    char *const tmp = malloc(len + 1);

    if (tmp == nullptr) {
        return false;
    }

    fbuf->lines[fbuf->count].line = tmp;
    memcpy(fbuf->lines[fbuf->count].line, line, len);
    fbuf->lines[fbuf->count].line[len] = '\0';
    fbuf->lines[fbuf->count].size = len + 1;
    return fbuf->count + 1 < fbuf->count ? false : ++fbuf->count;
}
