#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#define _POSIX_C_SOURCE 2008'09L
#define _XOPEN_SOURCE   700

#include <stdio.h>

#include "cread/test/acutest.h"
#include "cread/test/stubs.c"

#include "cread/src/FileBuf.h"
#include "cread/src/readlines_getline.c"
#include "cread/src/readlines_mmap_getline.c"

void test_readlines_mmap_getline_failure(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_mmap_getline(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_readlines_mmap_getline_failure", test_readlines_mmap_getline_failure},
    { nullptr, nullptr }
};
