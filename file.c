#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file.h"


FILE *file_open(const char *filepath, const char *mode) {
    if (!filepath) {
        perror("Filepath is required.");
        return NULL;
    }

    FILE *file = fopen(filepath, mode);
    if (!file) {
        perror("File does not found.");
        return NULL;
    }

    return file;
}

int file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}

// int file_row_starts_with(FILE *file, char ch) { }

char *file_read_into_memory(FILE *file) {
    int size = file_size(file);

    char *buf = (char*)malloc(size + 1);
    if (!buf) {
        perror("Cannot allocate memory");
        return NULL;
    }

    size_t bytes = fread(buf, 1, size, file);

    if (bytes != (size_t)size) {
        perror("Cannot read file into memory: size is different");
        return NULL;
    }

    buf[size] = '\0';
    return buf;
}
