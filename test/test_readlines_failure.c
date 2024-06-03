#undef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE

#define _POSIX_C_SOURCE 2008'19L
#define _XOPEN_SOURCE   700

#include <stdio.h>

#include "cread/src/FileBuf.h"

#include "cread/test/acutest.h"
#include "cread/test/stubs.c"

#ifdef WITH_FREAD
    #define TESTME readlines_fread
    #include "cread/src/readlines_fread.c"
#elifdef WITH_GETLINE
    #define TESTME readlines_getline
    #include "cread/src/readlines_getline.c"
#elifdef WITH_MMAP_GETLINE
    #define TESTME readlines_mmap_getline
    #include "cread/src/readlines_getline.c"
    #include "cread/src/readlines_mmap_getline.c"
#elifdef WITH_MMAP_MEMCHR
    #define TESTME readlines_mmap_memchr
    #include "cread/src/readlines_mmap_memchr.c"
#elifdef WITH_READ
    #define TESTME readlines_read
    #include "cread/src/readlines_read.c"
#else
    #error "You need to specify one of WITH_FREAD, WITH_GETLINE,"
           "WITH_MMAP_GETLINE, WITH_MMAP_MEMCHR, or WITH_READ."
#endif

#define STRINGIFY_INDIRECT(x)       #x
#define STRINGIFY(x)                STRINGIFY_INDIRECT(x)

#define CONCAT_INDIRECT(a, b, c)    a ## b ## c
#define CONCAT(a, b, c)             CONCAT_INDIRECT(a, b, c)

void CONCAT(test_, TESTME, _failure)(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!TESTME(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_"  STRINGIFY(TESTME) "_failure",  CONCAT(test_, TESTME, _failure) },
    { nullptr, nullptr },
};
