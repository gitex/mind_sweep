#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file.h"


FILE *file_open(const char *filepath, const char *mode) {
    if (!filepath) {
        fprintf(stderr, "Filepath is required\n");
        return NULL;
    }

    FILE *file = fopen(filepath, mode);
    if (!file) {
        fprintf(stderr, "File does not found\n");
        return NULL;
    }

    return file;
}

long file_size(FILE *file) {
    if (fseek(file, 0, SEEK_END) != 0) return -1;
    long size = ftell(file);
    rewind(file);
    return size;
}

char *file_read_into_memory(FILE *file) {
    int size = file_size(file);

    char *buf = malloc(size + 1);
    if (!buf) {
        fprintf(stderr, "Cannot allocate memory\n");
        return NULL;
    }

    size_t bytes = fread(buf, 1, size, file);

    if (bytes != (size_t)size) {
        fprintf(stderr, "Cannot read file into memory: size is different\n");
        free(buf);
        return NULL;
    }

    buf[size] = '\0';
    return buf;
}
