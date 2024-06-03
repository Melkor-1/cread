CC = gcc-13

CFLAGS += -I..
CFLAGS += -std=c2x

ifneq ($(findstring debug,$(MAKECMDGOALS)),)
	CFLAGS += -g3
	CFLAGS += -ggdb

	CFLAGS += -no-pie

	CFLAGS += -fno-builtin
	CFLAGS += -fno-common
	CFLAGS += -fno-omit-frame-pointer

	CFLAGS += -Wall
	CFLAGS += -Wextra
	CFLAGS += -Warray-bounds

	CFLAGS += -Wconversion
	CFLAGS += -Wformat-signedness
	CFLAGS += -Wno-parentheses
	CFLAGS += -Wpedantic
	CFLAGS += -pedantic-errors
	CFLAGS += -Wstrict-prototypes
	CFLAGS += -Wwrite-strings
	CFLAGS += -Wno-missing-braces
	CFLAGS += -Wno-missing-field-initializers

	CFLAGS += -fsanitize=address
	CFLAGS += -fsanitize=undefined
	CFLAGS += -fsanitize=bounds-strict
	CFLAGS += -fsanitize=leak
	CFLAGS += -fsanitize=null
	CFLAGS += -fsanitize=signed-integer-overflow
	CFLAGS += -fsanitize=bool
	CFLAGS += -fsanitize=pointer-overflow
	CFLAGS += -fsanitize-address-use-after-scope

	CFLAGS += -fanalyzer
endif

ifneq ($(findstring release,$(MAKECMDGOALS)),)
	CFLAGS += -pedantic
	CFLAGS += -O3
	CFLAGS += -march=native
	CFLAGS += -s
endif

ifneq ($(findstring test,$(MAKECMDGOALS)),)
	CFLAGS += -Wall
	CFLAGS += -Wextra
endif

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR = obj
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

TEST_SRC_DIR = test
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_BIN_DIR = test/bin

TARGET = read_file

debug: $(TARGET)
release: $(TARGET)
	
$(TARGET): $(OBJS) | $(OBJ_DIR) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

-include $(DEPS)

valgrind: clean $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --mmap_memchr $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --getline $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --mmap_getline $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --fread $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --read $(TARGET)

benchmark: $(TARGET)
	./benchmark $(TARGET) > log.txt

$(TEST_BIN_DIR): $(OBJS) | $(OBJ_DIR)
	mkdir -p $(TEST_BIN_DIR)

	$(CC) $(CFLAGS) -DWITH_FREAD -DFEOF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_fread_feof_failure
	$(CC) $(CFLAGS) -DWITH_FREAD -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_fread_resize_fbuf_failure
	$(CC) $(CFLAGS) -DWITH_FREAD -DALLOC_AND_APPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_fread_alloc_and_append_line_failure
	$(CC) $(CFLAGS) -DWITH_FREAD $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_fread.o $(TEST_SRC_DIR)/test_readlines_success.c -o $(TEST_BIN_DIR)/test_readlines_fread_success

	$(CC) $(CFLAGS) -DWITH_GETLINE -DGETLINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_getline_getline_failure
	$(CC) $(CFLAGS) -DWITH_GETLINE -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_getline_resize_fbuf_failure
	$(CC) $(CFLAGS) -DWITH_GETLINE -DAPPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_getline_append_line_failure
	$(CC) $(CFLAGS) -DWITH_GETLINE $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_getline.o $(TEST_SRC_DIR)/test_readlines_success.c -o $(TEST_BIN_DIR)/test_readlines_getline_success

	$(CC) $(CFLAGS) -DWITH_MMAP_MEMCHR -DFSTAT $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_memchr_fstat_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_MEMCHR -DMMAP $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_memchr_mmap_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_MEMCHR -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_memchr_resize_fbuf_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_MEMCHR -DAPPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_memchr_append_line_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_MEMCHR $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_mmap_memchr.o $(TEST_SRC_DIR)/test_readlines_success.c -o $(TEST_BIN_DIR)/test_readlines_mmap_memchr_success

	$(CC) $(CFLAGS) -DWITH_MMAP_GETLINE -DFSTAT $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_getline_fstat_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_GETLINE -DMMAP $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_getline_mmap_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_GETLINE -DFMEMOPEN $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_getline_fmemopen_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_GETLINE -DGETLINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_mmap_getline_readlines_getline_failure
	$(CC) $(CFLAGS) -DWITH_MMAP_GETLINE $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_getline.o $(OBJ_DIR)/readlines_mmap_getline.o $(TEST_SRC_DIR)/test_readlines_success.c -o $(TEST_BIN_DIR)/test_readlines_mmap_getline_success

	$(CC) $(CFLAGS) -DWITH_READ -DFILENO $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_read_fstat_failure
	$(CC) $(CFLAGS) -DWITH_READ -DREAD $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_read_mmap_failure
	$(CC) $(CFLAGS) -DWITH_READ -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_read_resize_fbuf_failure
	$(CC) $(CFLAGS) -DWITH_READ -DALLOC_AND_APPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/test_readlines_failure.c -o $(TEST_BIN_DIR)/test_readlines_read_append_line_failure
	$(CC) $(CFLAGS) -DWITH_READ $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_read.o $(TEST_SRC_DIR)/test_readlines_success.c -o $(TEST_BIN_DIR)/test_readlines_read_success

test: $(TEST_BIN_DIR) $(OBJS) $(TEST_SRCS)
	@for test in $(wildcard $(TEST_BIN_DIR)/*); do \
		echo "Running $$test";	\
		./$$test;	\
	done

clean:
	$(RM) -r $(TARGET) $(OBJ_DIR)

tclean:
	$(RM) -r $(TEST_BIN_DIR)

clean-all:
	$(RM) -r $(TARGET) $(OBJ_DIR) $(TEST_BIN_DIR)

.PHONY: debug release valgrind benchmark test clean tclean clean-all
.DELETE_ON_ERROR:
