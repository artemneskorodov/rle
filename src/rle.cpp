#include "rle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static long file_size(FILE *file);

static const size_t MAX_ELEM_SIZE = 64;

char *rle_encode(const char *input_buffer, size_t input_size, size_t *outsize) {
    assert(input_buffer != NULL);
    assert(input_size   != NULL);
    assert(outsize      != NULL);
    *outsize = 0;
    size_t current_size = input_size * 2;
    char *encoded = (char *)calloc(current_size, sizeof(char));
    for(size_t i = 0; i < input_size; ) {
        size_t counter = 0;
        for(size_t j = i; j < input_size && input_buffer[j] == input_buffer[i]; j++, counter++);
        char elem[MAX_ELEM_SIZE] = {};
        sprintf(elem, "%llu%c", counter, input_buffer[i]);
        strcat(encoded + *outsize, elem);
        *outsize += strlen(elem);
        if(*outsize >= current_size / 2) {
            char *new_encoded = (char *)realloc(encoded, current_size * 2);
            if(new_encoded == NULL) {
                free(encoded);
                encoded = NULL;
                current_size = 0;
                return NULL;
            }
            memset(new_encoded + current_size, 0, current_size);
            current_size *= 2;
            encoded = new_encoded;
        }
        i += counter;
    }
    return encoded;
}

char *read_file(FILE *input, size_t *outsize) {
    assert(input   != NULL);
    assert(outsize != NULL);
    long size = file_size(input);
    if(size < 0) {
        return NULL;
    }

    char *buffer = (char *)calloc(size + 1, sizeof(char));
    if(buffer == NULL) {
        return NULL;
    }

    if(fread(buffer, sizeof(char), size, input) != (size_t)size) {
        free(buffer);
        buffer = NULL;
        return NULL;
    }

    *outsize = size;

    return buffer;
}

long file_size(FILE *file) {
    assert(file != NULL);
    if(fseek(file, 0, 2) != 0) {
        return -1;
    }
    long size = ftell(file);
    if(size < 0) {
        return -1;
    }
    if(fseek(file, 0, 0) != 0) {
        return -1;
    }
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
        char c = 0;
        while(isdigit(encoded[i])) {
            counter = 10 * counter + (encoded[i] - '0');
            i++;
        }
        c = encoded[i];
        if(*outsize + counter >= current_size / 2) {
            char *new_decoded = (char *)realloc(decoded, current_size * 2);
            if(new_decoded == NULL) {
                decoded = NULL;
                return NULL;
            }
            memset(new_decoded + current_size, 0, current_size);
            decoded = new_decoded;
            current_size *= 2;
        }
        for(size_t j = 0; j < counter; j++, *outsize += 1)
            decoded[*outsize] = c;
        i++;
    }
    return decoded;
}
