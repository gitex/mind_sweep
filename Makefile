build: exe

.PHONY: build exe debug clean gdb valgrind

CC = cc
CFILES = console.c file.c buffer.c main.c
HFILES = console.h file.h buffer.h
OUT_FILE = main
CFLAGS = -Wall -Wextra -Wpedantic

exe: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) $(CFILES) -o $(OUT_FILE)

debug: $(CFILES) $(HFILES)
	$(CC) -g -O0 $(CFLAGS) $(CFILES) -o $(OUT_FILE)

clean:
	-rm $(OUT_FILE) 2>/dev/null

gdb: main.c debug
	gdb ./$(OUT_FILE)

valgrind: main.c debug
	valgrind --leak-check=full ./$(OUT_FILE)

