CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

SRC = src/allocator.c
TEST = tests/allocator_test.c
INCLUDE = -Iinclude

all: test

test: $(SRC) $(TEST)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) $(TEST) -o test

run: test
	./test

clean:
	rm -f test