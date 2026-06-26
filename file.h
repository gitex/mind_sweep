#ifndef _FILE
#define _FILE

#include <stdio.h>

FILE *file_open(const char *filepath, const char *mode);
long file_size(FILE *file);
char *file_read_into_memory(FILE *file);

#endif
