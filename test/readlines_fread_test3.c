#include <stdio.h>

#include "acutest.h"

#define FEOF
#include "stubs.c"

#include "../src/FileBuf.h"
#include "../src/readlines_fread.c"

void test_read_error(void)
{
    FILE *const fp = fopen("test/stubs.c", "r");
    TEST_ASSERT(fp);
    TEST_CHECK(!readlines_fread(fp, &(FileBuf) {}));
}

TEST_LIST = {
    { "test_read_error", test_read_error},
    { nullptr, nullptr }
};
