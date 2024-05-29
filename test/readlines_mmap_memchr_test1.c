#include <stdio.h>

#include "acutest.h"

#define FSTAT 
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_mmap_memchr.c"

void test_get_file_status_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_mmap_memchr(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_get_file_status_error", test_get_file_status_error},
    { nullptr, nullptr }
};
