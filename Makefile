all: clean dev

.PHONY: all build run debug clean gdb valgrind

CC = gcc
CC_FLAGS = -std=c99 -pedantic -Wall -Wextra  -Wfloat-equal -Wformat=2 -Wshadow \
		   -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
		   -Wuninitialized -Wunused -Wwrite-strings -Werror

C_FILES = console.c file.c buffer.c main.c
H_FILES = console.h file.h buffer.h

TARGET = main

dev: $(C_FILES) $(H_FILES)
	$(CC) -O0 -g1 $(CC_FLAGS) $(C_FILES) -o $(TARGET)

release: $(C_FILES) $(H_FILES)
	$(CC) -O3 -DNDEBUG $(CC_FLAGS) $(C_FILES) -o $(TARGET)

debug: $(C_FILES) $(H_FILES)
	$(CC) -O0 -g3 -DDEBUG -fsanitize=address,undefined $(CC_FLAGS) $(C_FILES) -o $(TARGET)

run:
	./$(TARGET)

clean:
	-rm $(TARGET) 2>/dev/null

gdb: main.c debug
	gdb ./$(TARGET)

valgrind: main.c debug
	valgrind --leak-check=full ./$(TARGET)

