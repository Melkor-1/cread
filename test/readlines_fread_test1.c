#include <stdio.h>

#include "acutest.h"
#include "../src/common.h"

#define RESIZE_FBUF
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_fread.c"

void test_oom_and_overflow_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_fread(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_oom_and_overflow_error", test_oom_and_overflow_error },
    { nullptr, nullptr }
};
