#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

char *read_file(const char *filename, size_t *outsize) {
    assert(filename != NULL);
    assert(outsize  != NULL);

    FILE *input = fopen(filename, "r");
    if(input == NULL)
        return NULL;

    long size = file_size(input);
    if(size < 0)
        return NULL;

    *outsize = (size_t)size;
    char *buffer = (char *)calloc(*outsize + 1, sizeof(char));
    if(buffer == NULL)
        return NULL;

    if(fread(buffer, sizeof(char), *outsize, input) != *outsize) {
        free(buffer);
        buffer = NULL;
        return NULL;
    }

    fclose(input);
    return buffer;
}

//file of size (negative in case of error)
long file_size(FILE *file) {
    assert(file != NULL);
    if(fseek(file, 0, 2) != 0)
        return -1;

    long size = ftell(file);
    if(size < 0)
        return -1;

    if(fseek(file, 0, 0) != 0)
        return -1;

    return size;
}

//safe realloc, sets memory to 0
void *recalloc(void *memcell, size_t old_size, size_t new_size) {
    void *new_memcell = (char *)realloc(memcell, new_size);
    if(new_memcell == NULL) {
        free(memcell);
        return NULL;
    }
    memset((char *)new_memcell + old_size, 0, new_size - old_size);
    return new_memcell;
}

char *copy_string(const char *string, size_t size) {
    char *copy = (char *)calloc(size + 1, sizeof(char));
    if(copy == NULL)
        return NULL;
    memcpy(copy, string, size + 1);
    return copy;
}

size_t get_input(char *buffer, size_t max_size) {
    size_t size = 0;
    while((buffer[size++] = (char)getchar()) != '\n' && size < max_size);
    buffer[size--] = 0;
    return size;
}
