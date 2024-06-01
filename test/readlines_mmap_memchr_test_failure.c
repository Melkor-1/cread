#include <stdio.h>

#include "acutest.h"
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_mmap_memchr.c"

void test_readlines_mmap_memchr_failure(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_mmap_memchr(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_readlines_mmap_memchr_failure", test_readlines_mmap_memchr_failure},
    { nullptr, nullptr }
};
