#include <stdio.h>

#include "acutest.h"

#define MMAP 
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_getline.c"
#include "../src/readlines_mmap_getline.c"

void test_memory_mapping_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_mmap_getline(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_memory_mapping_error", test_memory_mapping_error},
    { nullptr, nullptr }
};
