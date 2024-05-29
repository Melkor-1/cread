#include <stdio.h>

#include "acutest.h"

#define FILENO 
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_read.c"

void test_fileno_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_read(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_fileno_error", test_fileno_error},
    { nullptr, nullptr }
};
