build: exe

CC = cc

exe: main.c
	$(CC) main.c -o main

debug: main.c
	$(CC) -g -O0 -Wall -Wextra main.c -o main

gdb: main.c debug
	gdb ./main

valgrind: main.c debug
	valgrind --leak-check=full ./main


