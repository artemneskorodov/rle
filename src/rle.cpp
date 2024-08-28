#include "rle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static long file_size(FILE *file);
static void *recalloc(void *memcell, size_t old_size, size_t new_size);

char *rle_encode(const char *input_buffer, size_t input_size, size_t *outsize) {
    assert(input_buffer != NULL);
    assert(input_size   != NULL);
    assert(outsize      != NULL);

    *outsize = 0;
    size_t current_size = input_size * 2;
    char *encoded = (char *)calloc(current_size, sizeof(char));

    for(size_t i = 0; i < input_size; ) {
        size_t counter = 0;
        while(i + counter < input_size && input_buffer[i + counter] == input_buffer[i])
            counter++;

        *outsize += sprintf(encoded + *outsize, "%llu%c", counter, input_buffer[i]);
        i += counter;

        if(*outsize >= current_size / 2) {
            size_t new_size = current_size * 2;
            while(*outsize > new_size / 4)
                new_size *= 2;

            char *new_encoded = (char *)recalloc(encoded, current_size, new_size);
            if(new_encoded == NULL)
                return NULL;

            current_size = new_size;
            encoded = new_encoded;
        }
    }
    return encoded;
}

char *read_file(FILE *input, size_t *outsize) {
    assert(input   != NULL);
    assert(outsize != NULL);
    long size = file_size(input);
    if(size < 0)
        return NULL;

    *outsize = (size_t)size;
    char *buffer = (char *)calloc(*outsize + 1, sizeof(char));
    if(buffer == NULL) {
        return NULL;
    }

    if(fread(buffer, sizeof(char), *outsize, input) != *outsize) {
        free(buffer);
        buffer = NULL;
        return NULL;
    }

    return buffer;
}

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

char *rle_decode(const char *encoded, size_t encoded_size, size_t *outsize) {
    assert(encoded != NULL);
    assert(outsize != NULL);

    *outsize = 0;
    size_t current_size = 2 * encoded_size;

    char *decoded = (char *)calloc(current_size, sizeof(char));
    if(decoded == NULL) {
        return NULL;
    }

    for(size_t i = 0; i < encoded_size; ) {
        size_t counter = 0;
        for(; isdigit(encoded[i]); i++)
            counter = 10 * counter + (encoded[i] - '0');
        char c = encoded[i++];

        if(*outsize + counter >= current_size / 2) {
            size_t new_size = current_size * 2;
            while(*outsize + counter > new_size / 4)
                new_size *= 2;

            char *new_decoded = (char *)recalloc(decoded, current_size, new_size);
            if(new_decoded == NULL)
                return NULL;

            decoded = new_decoded;
            current_size = new_size;
        }

        memset(decoded + *outsize, c, counter);
        *outsize += counter;

    }
    return decoded;
}

void *recalloc(void *memcell, size_t old_size, size_t new_size) {
    void *new_memcell = (char *)realloc(memcell, new_size);
    if(new_memcell == NULL) {
        free(memcell);
        return NULL;
    }
    memset((char *)new_memcell + old_size, 0, new_size - old_size);
    return new_memcell;
}
