/* Note: On Linux, SIGBUS is generated on an attempted access to a portion of 
 *       the buffer that does not correspond to the file (for example, beyond 
 *       the end of the file, including the case where another process has
 *       truncated the file). POSIX specification of mmap() does not require
 *       that the signal is delivered on error but leaves such possibility for
 *       implementations (out-of-space condition is provided as an example for
 *       such a signal being generated). The man page for OpenBSD states that 
 *       it generates a SIGSEGV signal instead of SIGBUS on some architectures.
 *       FreeBSD and NetBSD guarantee a SIGBUS signal. Oracle Solaris's man
 *       page specifies that either of these two signals can be generated.
 *
 *       Code using either readlines_mmap_memchr() or readlines_mmap_getline()
 *       might like to take the above into consideration. */

#define _POSIX_C_SOURCE 200'809L
#define _XOPEN_SOURCE   700

#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

typedef uint64_t timestamp_t;

typedef struct line {
    char *line;
    size_t size;
} Line;

typedef struct file_buf {
    Line *lines;
    size_t capacity;
    size_t count;
    size_t size;
} FileBuf;

[[gnu::always_inline, gnu::const]] static inline double mcs_to_secs(double mcs)
{
    return mcs / 1'000'000.0;
}

[[nodiscard, gnu::always_inline, gnu::returns_nonnull, gnu::nonnull]] static
inline void *safe_trim(void *p, size_t n)
{
    void *const cp = realloc(p, n);

    return cp ? cp : p;
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

void free_lines(FileBuf fbuf[static 1])
{
    for (size_t i = 0; i < fbuf->count; ++i) {
        free(fbuf->lines[i].line);
    }
    free(fbuf->lines);
}

/* If cap is less than 8, bumps it up to 8 and returns true.
 * Else it returns the result of `cap * 2` in result. 
 *
 * The function returns false if the multiplication overflowed, or true
 * elsewise. result is not modified in case of an overflow. */ 
[[gnu::always_inline, gnu::const]] static inline size_t grow_capacity(
        size_t result[static 1],
        size_t cap)
{
    if (cap < 8) {
        *result = 8;
        return true;
    }
    
    if (2 > SIZE_MAX / cap) {
        return false;
    }

    *result = cap * 2;
    return true; 
}

[[nodiscard, gnu::always_inline]] static inline bool resize_fbuf(
        FileBuf fbuf[static 1])
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

[[nodiscard]] bool append_line(FileBuf fbuf[static 1], 
                               size_t len,
                               char line[static len])
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

void mmap_memchr_cleanup(const FileBuf fbuf[static 1])
{
    munmap(fbuf->lines[0].line, fbuf->size);
    free(fbuf->lines);
}

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns true. `mmap_memchr_cleanup()` should be called by the 
 * calling code at some point to clean up all resources associated with this 
 * file.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool getlines_mmap_memchr(FILE *stream, 
                                                      FileBuf fbuf[static 1])
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

    char *lhs;
    char *rhs;

    for (lhs = map; lhs < &map[st.st_size]; lhs = rhs + 1) {
        rhs = memchr(lhs, '\n', (size_t) (&map[st.st_size] - lhs));

        if (rhs == nullptr) {
            /* We have reached end-of-file or the file is malformed. */
            break;
        }

        if (fbuf->capacity <= fbuf->count) {
            if (!resize_fbuf(fbuf)) {
                goto cleanup_and_fail;
            }
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

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns true. The calling code is required to call `free_lines()`
 * at some point in the code to release the memory associated with the lines.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool getlines_getline(FILE *stream, 
                                                  FileBuf fbuf[static 1])
{
    posix_fadvise(fileno(stream), 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED);

    char *line = nullptr;
    size_t capacity = 0;
    ssize_t nread = 0;

    for (;;) {
        nread = getline(&line, &capacity, stream);

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

        if (fbuf->capacity <= fbuf->count) {
            if (!resize_fbuf(fbuf)) {
                goto cleanup_and_fail;
            }
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
    free_lines(fbuf);
    return false;
}

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns true. The calling code is required to call `free_lines()`
 * at some point in the code to release the memory associated with the lines.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool getlines_mmap_getline(FILE *stream, 
                                                       FileBuf fbuf[static 1])
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

    const bool result = getlines_getline(memstream, fbuf);

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

/* Reads the next chunk of data from the stream referenced to by `stream`.
 * `chunk` must be a pointer to an array of size `size`. 
 *
 * Returns the number of bytes read on success, or zero elsewise. The chunk is 
 * not null-terminated.
 *
 * `read_next_chunk()` does not distinguish between end-of-file and error; the
 * routines `feof()` and `ferror()` must be used to determine which occured. */
[[nodiscard, gnu::nonnull]] size_t read_next_chunk(FILE *restrict stream, 
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

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns true. The calling code is required to call `free_lines()`
 * at some point in the code to release the memory associated with the lines.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool getlines_fread(FILE *stream, 
                                                FileBuf fbuf[static 1])
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
    size_t line_len= 0;
    char *lhs;
    char *rhs;

    while (nread = read_next_chunk(stream, CHUNK_SIZE - remain, chunk + remain)) {
        fbuf->size += nread;
        nread += remain;    /* Include remaining bytes from the previous read. */
        remain = 0;         /* Reset remain for the current iteration. */


        for (lhs = chunk; lhs < &chunk[nread]; lhs = rhs + 1) {
            rhs = memchr(lhs, '\n', (size_t) (&chunk[nread] - lhs));

            if (rhs == nullptr) {
                /* We did not find a newline. We shalt consider this an
                 * incomplete line and copy the bytes after the last '\n'
                 * to the beginning of the chunk. */
                remain = (size_t) (&chunk[nread] - lhs);
                memmove(chunk, chunk + (lhs - chunk), remain);
                break;
            }

            line_len = (size_t) (rhs - lhs);

            if (fbuf->capacity <= fbuf->count) {
                if (!resize_fbuf(fbuf)) {
                    goto cleanup_and_fail;
                }
            }

            if (!alloc_and_append_line(fbuf, line_len, lhs)) {
                goto cleanup_and_fail; 
            }
        }
    }
    
    if (remain) {
        if (fbuf->capacity <= fbuf->count) {
            if (!resize_fbuf(fbuf)) {
                goto cleanup_and_fail;
            }
        }

        if (!alloc_and_append_line(fbuf, remain, chunk)) {
            goto cleanup_and_fail; 
        }
    }

    /* Trim to maximum used. */
    fbuf->lines = safe_trim(fbuf->lines, fbuf->count * sizeof fbuf->lines[0]);
    return feof(stream);

  cleanup_and_fail:
    free_lines(fbuf);
    return false;
}

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
    fprintf(stream, "Usage: %s OPTIONS FILE.\n"
                    "OPTION: --fread, --mmap_getline, --mmap_memchr, --getline.\n",
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
    bool (*fn)(FILE *, FileBuf *) = {};

    if (strcmp(argv[1], "--getline") == 0) {
        fn = getlines_getline;
    } else if (strcmp(argv[1], "--mmap_getline") == 0) {
        fn = getlines_mmap_getline;
    } else if (strcmp(argv[1], "--mmap_memchr") == 0) {
        fn = getlines_mmap_memchr;
    } else if (strcmp(argv[1], "--fread") == 0) {
        fn = getlines_fread;
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

    if (!fn(stream, &fbuf)) {
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
    fn == getlines_mmap_memchr ? mmap_memchr_cleanup(&fbuf) : free_lines(&fbuf);
    fclose(stream);
    return EXIT_SUCCESS;
}
