CC = gcc-13

CFLAGS += -DBENCHMARKING

CFLAGS += -O3
CFLAGS += -std=gnu2x
CFLAGS += -s
CFLAGS += -no-pie

CFLAGS += -fno-builtin
CFLAGS += -fno-common
CFLAGS += -fno-omit-frame-pointer

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Warray-bounds
#CFLAGS += -Wconversion
CFLAGS += -Wformat-signedness
CFLAGS += -Wno-parentheses
CFLAGS += -Wpedantic
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

RM = /bin/rm

SRCS = $(wildcard src/*.c)
TARGET = read_file
COMMON_SRC = src/common.c
TEST_SRCS = $(wildcard test/readlines*.c)
TEST_BINS = $(patsubst %.c,%,$(TEST_SRCS))

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

valgrind: $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./read_file --mmap_memchr read_file
	valgrind --tool=memcheck --leak-check=yes ./read_file --getline read_file
	valgrind --tool=memcheck --leak-check=yes ./read_file --mmap_getline read_file
	valgrind --tool=memcheck --leak-check=yes ./read_file --fread read_file

benchmark: $(TARGET)
	./benchmark > log.txt
	
test: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "Running $$test"; \
		./$$test; \
	done

$(TEST_BINS): %: %.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET)

tclean:
	$(RM) $(TEST_BINS)

.PHONY: all clean valgrind benchmark test
.DELETE_ON_ERROR:

