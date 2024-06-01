#include <stdio.h>

#include "acutest.h"
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_getline.c"
#include "../src/readlines_mmap_getline.c"

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
