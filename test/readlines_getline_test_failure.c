#include <stdio.h>

#include "acutest.h"
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_getline.c"

void test_readlines_getline_failure(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_getline(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_readlines_getline_failure", test_readlines_getline_failure },
    { nullptr, nullptr }
};
