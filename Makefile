CC = gcc-13

CFLAGS += -I..

CFLAGS += -DBENCHMARKING

CFLAGS += -O3
CFLAGS += -std=c2x
CFLAGS += -s
CFLAGS += -no-pie

CFLAGS += -fno-builtin
CFLAGS += -fno-common
CFLAGS += -fno-omit-frame-pointer

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Warray-bounds

# This doesn't emit anything for our codebase, but does so for the testing header. 
# So disable it.
# CFLAGS += -Wconversion

CFLAGS += -Wformat-signedness
CFLAGS += -Wno-parentheses
CFLAGS += -Wpedantic
CFLAGS += -pedantic-errors
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wwrite-strings
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-missing-field-initializers

# CFLAGS += -fsanitize=address
# CFLAGS += -fsanitize=undefined
# CFLAGS += -fsanitize=bounds-strict
# CFLAGS += -fsanitize=leak
# CFLAGS += -fsanitize=null
# CFLAGS += -fsanitize=signed-integer-overflow
# CFLAGS += -fsanitize=bool
# CFLAGS += -fsanitize=pointer-overflow
# CFLAGS += -fsanitize-address-use-after-scope

# CFLAGS += -fanalyzer

MKDIR = /bin/mkdir -p

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR = obj
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

TEST_SRC_DIR = test
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_BIN_DIR = test/bin

TARGET = read_file

all: $(TARGET)
	
$(TARGET): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

-include $(DEPS)

valgrind: $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --mmap_memchr $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --getline $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --mmap_getline $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --fread $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./$(TARGET) --read $(TARGET)

benchmark: $(TARGET)
	./benchmark $(TARGET) > log.txt

$(TEST_BIN_DIR): $(OBJ_DIR) $(OBJS)
	$(MKDIR) $(TEST_BIN_DIR)

	$(CC) $(CFLAGS) -DFEOF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_fread_test_failure.c -o $(TEST_BIN_DIR)/readlines_fread_test_failure_feof
	$(CC) $(CFLAGS) -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_fread_test_failure.c -o $(TEST_BIN_DIR)/readlines_fread_test_failure_resize_fbuf
	$(CC) $(CFLAGS) -DALLOC_AND_APPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_fread_test_failure.c -o $(TEST_BIN_DIR)/readlines_fread_test_failure_alloc_and_append_line
	$(CC) $(CFLAGS) -DWITH_FREAD $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_fread.o $(TEST_SRC_DIR)/readlines_all_test_success.c -o $(TEST_BIN_DIR)/readlines_fread_test_success

	$(CC) $(CFLAGS) -DGETLINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_getline_test_failure_getline
	$(CC) $(CFLAGS) -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_getline_test_failure_resize_fbuf
	$(CC) $(CFLAGS) -DAPPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_getline_test_failure_append_line
	$(CC) $(CFLAGS) -DWITH_GETLINE $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_getline.o $(TEST_SRC_DIR)/readlines_all_test_success.c -o $(TEST_BIN_DIR)/readlines_getline_test_success

	$(CC) $(CFLAGS) -DFSTAT $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_memchr_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_memchr_test_failure_fstat
	$(CC) $(CFLAGS) -DMMAP $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_memchr_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_memchr_test_failure_mmap
	$(CC) $(CFLAGS) -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_memchr_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_memchr_test_failure_resize_fbuf
	$(CC) $(CFLAGS) -DAPPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_memchr_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_memchr_test_failure_append_line
	$(CC) $(CFLAGS) -DWITH_MMAP_MEMCHR $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_mmap_memchr.o $(TEST_SRC_DIR)/readlines_all_test_success.c -o $(TEST_BIN_DIR)/readlines_mmap_memchr_test_success

	$(CC) $(CFLAGS) -DFSTAT $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_getline_test_failure_fstat
	$(CC) $(CFLAGS) -DMMAP $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_getline_test_failure_mmap
	$(CC) $(CFLAGS) -DFMEMOPEN $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_getline_test_failure_fmemopen
	$(CC) $(CFLAGS) -DGETLINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_mmap_getline_test_failure.c -o $(TEST_BIN_DIR)/readlines_mmap_getline_test_failure_readlines_getline
	$(CC) $(CFLAGS) -DWITH_MMAP_GETLINE $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_getline.o $(OBJ_DIR)/readlines_mmap_getline.o $(TEST_SRC_DIR)/readlines_all_test_success.c -o $(TEST_BIN_DIR)/readlines_mmap_getline_test_success

	$(CC) $(CFLAGS) -DFILENO $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_read_test_failure.c -o $(TEST_BIN_DIR)/readlines_read_test_failure_fstat
	$(CC) $(CFLAGS) -DREAD $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_read_test_failure.c -o $(TEST_BIN_DIR)/readlines_read_test_failure_mmap
	$(CC) $(CFLAGS) -DRESIZE_FBUF $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_read_test_failure.c -o $(TEST_BIN_DIR)/readlines_read_test_failure_resize_fbuf
	$(CC) $(CFLAGS) -DALLOC_AND_APPEND_LINE $(OBJ_DIR)/common.o $(TEST_SRC_DIR)/readlines_read_test_failure.c -o $(TEST_BIN_DIR)/readlines_read_test_failure_append_line
	$(CC) $(CFLAGS) -DWITH_READ $(OBJ_DIR)/common.o $(OBJ_DIR)/readlines_read.o $(TEST_SRC_DIR)/readlines_all_test_success.c -o $(TEST_BIN_DIR)/readlines_read_test_success

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

.PHONY: all valgrind benchmark test clean tclean clean-all
.DELETE_ON_ERROR:
