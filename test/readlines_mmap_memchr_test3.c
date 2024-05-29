#include <stdio.h>

#include "acutest.h"

#define RESIZE_FBUF 
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_mmap_memchr.c"

void test_oom_and_overflow_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_mmap_memchr(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_oom_and_overflow_error", test_oom_and_overflow_error},
    { nullptr, nullptr }
};
