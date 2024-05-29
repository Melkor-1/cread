#include <stdio.h>

#include "acutest.h"

#define ALLOC_AND_APPEND_LINE 
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_read.c"

void test_oom_and_overflow_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_read(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_oom_and_overflow_error", test_oom_and_overflow_error},
    { nullptr, nullptr }
};
