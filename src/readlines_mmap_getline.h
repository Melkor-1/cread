#ifndef READLINES_MMAP_GETLINE
#define READLINES_MMAP_GETLINE 1

/* Note: On Linux, `SIGBUS` is generated on an attempted access to a portion of 
 *       the buffer that does not correspond to the file (for example, beyond 
 *       the end of the file, including the case where another process has
 *       truncated the file). POSIX specification of `mmap()` does not require
 *       that the signal is delivered on error but leaves such possibility for
 *       implementations (out-of-space condition is provided as an example for
 *       such a signal being generated). The man page for OpenBSD states that 
 *       it generates a `SIGSEGV` signal instead of `SIGBUS` on some architectures.
 *       FreeBSD and NetBSD guarantee a `SIGBUS` signal. Oracle Solaris's man
 *       page specifies that either of these two signals can be generated.
 *
 *       Code using `readlines_mmap_getline()` might like to take the above into 
 *       consideration. */

#include <stdio.h>

#include "common.h"

/* `stream` should be valid, i.e. associated with a file, and `fbuf` should 
 * point to a valid object of type `FileBuf`.
 *
 * On success, returns `true`. The calling code is required to call 
 * `readlines_mmap_getline_cleanup()` at some point in the code to release the
 * memory associated with the lines.
 *
 * On failure, all resources are released (`stream` is not touched) and `false`
 * is returned. */
[[nodiscard, gnu::nonnull]] bool readlines_mmap_getline(FILE *stream, 
                                                        FileBuf fbuf[static 1]);

void readlines_mmap_getline_cleanup(FileBuf fbuf[static 1]);

#endif  /* READLINES_MMAP_GETLINE */
