#include <stdio.h>

#include "acutest.h"
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_read.c"

void test_readlines_test_failure(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_read(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_readlines_test_failure", test_readlines_test_failure},
    { nullptr, nullptr }
};
