#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include <stdio.h>
#include <string.h>

#include "cread/test/acutest.h"

#include "cread/src/FileBuf.h"

#ifdef WITH_FREAD
    #define TESTME  readlines_fread
    #include "cread/src/readlines_fread.h"
#elifdef WITH_GETLINE
    #define TESTME  readlines_getline
    #include "cread/src/readlines_getline.h"
#elifdef WITH_MMAP_GETLINE
    #define TESTME  readlines_mmap_getline
    #include "cread//src/readlines_getline.h"
    #include "cread/src/readlines_mmap_getline.h"
#elifdef WITH_MMAP_MEMCHR
    #define TESTME  readlines_mmap_memchr
    #include "cread/src/readlines_mmap_memchr.h"
#elifdef WITH_READ
    #define TESTME  readlines_read
    #include "cread/src/readlines_read.h"
#else
    #error "You need to specify one of WITH_FREAD, WITH_GETLINE,"
           "WITH_MMAP_GETLINE, WITH_MMAP_MEMCHR, or WITH_READ."
#endif

#define STRINGIFY_INDIRECT(x)   #x
#define STRINGIFY(x)            STRINGIFY_INDIRECT(x)

#define CONCAT_INDIRECT2(a, b)  a ## b 
#define CONCAT2(a, b)           CONCAT_INDIRECT2(a, b)

#define CONCAT_INDIRECT3(a, b, c)   a ## b ## c
#define CONCAT3(a, b, c)            CONCAT_INDIRECT3(a, b, c)

void CONCAT3(test_, TESTME, _success)(void)
{
    /* The lines and line lengths are at most 1024 in sample1.txt. */
    char buf[1024];
    char *lines[1024];
    size_t line_count = 0;
    FILE *const fp = fopen("test/sample.txt", "r");

    TEST_ASSERT(fp);

    while (fgets(buf, sizeof buf, fp)) {
        buf[strcspn(buf, "\n")] = '\0';
        TEST_ASSERT((lines[line_count] = strdup(buf)));
        ++line_count;
    }
    
    rewind(fp);

    FileBuf fbuf = {};

    TEST_CHECK(TESTME(fp, &fbuf));
    TEST_CHECK(fbuf.count == 555);
    TEST_CHECK(fbuf.size == 120635);

    for (size_t i = 0; i < fbuf.count; ++i) {
        TEST_CHECK((strcmp(lines[i], fbuf.lines[i].line) == 0));
    }

    fclose(fp);
}

TEST_LIST = {
    { "test_"  STRINGIFY(TESTME) "_success",  CONCAT3(test_, TESTME, _success) },
    { nullptr, nullptr },
};
