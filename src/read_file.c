#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

#include "cread/src/FileBuf.h"
#include "cread/src/readlines_fread.h"
#include "cread/src/readlines_getline.h"
#include "cread/src/readlines_mmap_getline.h"
#include "cread/src/readlines_mmap_memchr.h"
#include "cread/src/readlines_read.h"

typedef uint64_t timestamp_t;

[[gnu::always_inline, gnu::const]] static inline double mcs_to_secs(double mcs)
{
    return mcs / 1'000'000.0;
}

#ifndef BENCHMARKING
void print_lines(const FileBuf fbuf[static 1])
{
    for (size_t i = 0; i < fbuf->count; i++) {
        puts(fbuf->lines[i].line);
    }
}

[[gnu::always_inline, gnu::pure]] static inline size_t get_total_lines(
        const FileBuf fbuf[static 1])
{
    return fbuf->count;
}

[[gnu::always_inline, gnu::pure]] static inline size_t get_total_bytes(
        const FileBuf fbuf[static 1])
{
    return fbuf->size;
}
#endif

[[gnu::always_inline]] static inline timestamp_t get_posix_clock_time_fallback(void)
{
    struct timeval tv;
    return gettimeofday (&tv, NULL) == 0 ? 
        (timestamp_t) tv.tv_sec * 1'000'000 + (timestamp_t) tv.tv_usec : 0;
}

/* Reference: https://stackoverflow.com/a/37920181/20017547 */
[[gnu::always_inline]] static inline timestamp_t get_posix_clock_time(void)
{
#ifdef _POSIX_MONOTONIC_CLOCK
    struct timespec ts;

    if (clock_gettime (CLOCK_MONOTONIC, &ts) == 0) {
        return (timestamp_t) (ts.tv_sec * 1'000'000 + ts.tv_nsec / 1'000);
    } 
    return get_posix_clock_time_fallback();
#else
    return get_posix_clock_time_fallback();
#endif /* _POSIX_MONOTONIC_CLOCK */
}

[[gnu::always_inline, gnu::const]] static inline timestamp_t get_clock_difftime(
        timestamp_t t0, 
        timestamp_t t1)
{
    return t1 - t0;
}

[[gnu::nonnull, gnu::always_inline]] static inline void usage(
        FILE *restrict stream, 
        const char argv0[restrict static 1])
{
    fprintf(stream, "\nUsage: %s OPTIONS FILE.\n\n"
            "OPTIONS:\n"
            "   --fread\n"
            "   --mmap_getline\n"
            "   --mmap_memchr\n"
            "   --getline\n"
            "   --read\n",
            argv0);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "error: expected 3 arguments.\n");
        usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    FileBuf fbuf = {};
    bool (*read_fn)(FILE *, FileBuf *) = {};
    void (*free_fn)(FileBuf *) = {};

    if (strcmp(argv[1], "--getline") == 0) {
        read_fn = readlines_getline;
        free_fn = readlines_getline_cleanup;
    } else if (strcmp(argv[1], "--mmap_getline") == 0) {
        read_fn = readlines_mmap_getline;
        free_fn = readlines_mmap_getline_cleanup;
    } else if (strcmp(argv[1], "--mmap_memchr") == 0) {
        read_fn = readlines_mmap_memchr;
        free_fn = readlines_mmap_memchr_cleanup;
    } else if (strcmp(argv[1], "--fread") == 0) {
        read_fn = readlines_fread;
        free_fn = readlines_fread_cleanup;
    } else if (strcmp(argv[1], "--read") == 0) {
        read_fn = readlines_read;
        free_fn = readlines_read_cleanup;
    } else {
        fprintf(stderr, "Unknown option: \"%s\".\n", argv[1]);
        usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    FILE *const stream = fopen(argv[2], "rb");

    if (stream == nullptr) {
        perror(argv[2]);
        return EXIT_FAILURE;
    }
    
    const timestamp_t t0 = get_posix_clock_time();

    if (!read_fn(stream, &fbuf)) {
        fprintf(stderr, "error: failed to read file: %s.\n",
            strerror(errno));
        fclose(stream);
        return EXIT_FAILURE;
    }

    const timestamp_t msecs = get_clock_difftime(t0, get_posix_clock_time());
    
#ifndef BENCHMARKING 
    print_lines(&fbuf);
    fprintf(stderr, "Read %ju lines, %ju bytes in: %f secs.\n", 
                    get_total_lines(&fbuf),
                    get_total_bytes(&fbuf),
                    mcs_to_secs((double) msecs));
#else
    printf("%fs", mcs_to_secs((double) msecs));
#endif
    free_fn(&fbuf);
    fclose(stream);
    return EXIT_SUCCESS;
}
