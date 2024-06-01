#include <stdio.h>

#include "acutest.h"
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_fread.c"

void test_readlines_fread_failure(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_fread(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_readlines_fread_failure", test_readlines_fread_failure},
    { nullptr, nullptr }
};
