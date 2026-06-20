build: exe

CC = cc

exe: main.c console.c console.h
	$(CC) main.c console.c -o main

debug: main.c console.c console.h
	$(CC) -g -O0 -Wall -Wextra main.c console.c -o main

gdb: main.c debug
	gdb ./main

valgrind: main.c debug
	valgrind --leak-check=full ./main

