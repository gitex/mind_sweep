all: clean dev

.PHONY: all build run debug clean gdb valgrind

CC = gcc
CC_FLAGS = -Wall -Wextra -Wpedantic

C_FILES = console.c file.c buffer.c main.c
H_FILES = console.h file.h buffer.h

TARGET = main

dev: $(C_FILES) $(H_FILES)
	$(CC) -O0 -g1 $(CC_FLAGS) $(C_FILES) -o $(TARGET)

release: $(C_FILES) $(H_FILES)
	$(CC) -O2 $(CC_FLAGS) $(C_FILES) -o $(TARGET)

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

