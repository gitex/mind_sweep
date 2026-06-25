build: exe

CC = cc
C_FILES = console.c file.c tree.c main.c
H_FILES = console.h file.h tree.h
OUT_FILE = main

exe: $(C_FILES) $(H_FILES)
	$(CC) $(C_FILES) -o $(OUT_FILE)

debug: $(C_FILES) $(H_FILES)
	$(CC) -g -O0 -Wall -Wextra $(C_FILES) -o $(OUT_FILE)

clear:
	-rm $(OUT_FILE) 2>/dev/null

gdb: main.c debug
	gdb ./$(OUT_FILE)

valgrind: main.c debug
	valgrind --leak-check=full ./$(OUT_FILE)

