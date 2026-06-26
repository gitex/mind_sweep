#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file.h"


FILE *file_open(const char *filepath, const char *mode) {
    if (!filepath) {
        fprintf(stderr, "Filepath is required.");
        return NULL;
    }

    FILE *file = fopen(filepath, mode);
    if (!file) {
        fprintf(stderr, "File does not found.");
        return NULL;
    }

    return file;
}

// TODO: change return to size_t or long int
int file_size(FILE *file) {
    // TODO: handle errors of fseek and ftell, return negative codes
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}


char *file_read_into_memory(FILE *file) {
    int size = file_size(file);

    char *buf = (char*)malloc(size + 1);
    if (!buf) {
        fprintf(stderr, "Cannot allocate memory");
        return NULL;
    }

    size_t bytes = fread(buf, 1, size, file);

    if (bytes != (size_t)size) {
        fprintf(stderr, "Cannot read file into memory: size is different");
        free(buf);
        return NULL;
    }

    buf[size] = '\0';
    return buf;
}
