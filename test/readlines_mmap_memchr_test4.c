#include <stdio.h>

#include "acutest.h"

#define APPEND_LINE 
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_mmap_memchr.c"

void test_overflow_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_mmap_memchr(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_overflow_error", test_overflow_error},
    { nullptr, nullptr }
};