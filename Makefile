CC = gcc-13

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
CFLAGS += -Wconversion
CFLAGS += -Wformat-signedness
CFLAGS += -Wno-parentheses
CFLAGS += -Wpedantic
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wwrite-strings
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-missing-field-initializers

CFLAGS += -Wsuggest-attribute=pure
CFLAGS += -Wsuggest-attribute=const
CFLAGS += -Wsuggest-attribute=noreturn
CFLAGS += -Wsuggest-attribute=cold
CFLAGS += -Wsuggest-attribute=malloc
CFLAGS += -Wsuggest-attribute=format

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

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

valgrind: $(TARGET)
	valgrind --tool=memcheck --leak-check=yes ./read_file --mmap_memchr data/F_01.txt
	valgrind --tool=memcheck --leak-check=yes ./read_file --getline data/F_01.txt
	valgrind --tool=memcheck --leak-check=yes ./read_file --mmap_getline data/F_01.txt
	valgrind --tool=memcheck --leak-check=yes ./read_file --fread data/F_01.txt

benchmark: $(TARGET)
	./benchmark > log.txt
	
clean:
	$(RM) $(TARGET)

.PHONY: all clean valgrind benchmark
.DELETE_ON_ERROR:

