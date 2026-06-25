#include <stdio.h>


FILE *file_open(const char *filepath, const char *mode);
int file_size(FILE *file);
char *file_read_into_memory(FILE *file);
