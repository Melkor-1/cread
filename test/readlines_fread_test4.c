#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "acutest.h"

#include "../src/FileBuf.h"
#include "../src/readlines_fread.c"

void test_readlines_fread_correctness(void)
{
    /* The lines and line lengths are at most 1024 in sample1.txt. */
    char buf[1024];
    char *lines[1024];
    size_t line_count = 0;
    FILE *const fp = fopen("test/sample.txt", "r");

    assert(fp);

    while (fgets(buf, sizeof buf, fp)) {
        buf[strcspn(buf, "\n")] = '\0';
        assert(lines[line_count] = strdup(buf));
        ++line_count;
    }
    
    rewind(fp);

    FileBuf fbuf = {};

    assert(readlines_fread(fp, &fbuf));
    assert(fbuf.count == 555);
    assert(fbuf.size == 120635);

    for (size_t i = 0; i < fbuf.count; ++i) {
        assert(strcmp(lines[i], fbuf.lines[i].line) == 0);
    }

    fclose(fp);
}

TEST_LIST = {
    { "test_readlines_fread_correctness", test_readlines_fread_correctness },
    { nullptr, nullptr },
};
