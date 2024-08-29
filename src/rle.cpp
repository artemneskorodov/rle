#include "rle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static long file_size(FILE *file);
static void *recalloc(void *memcell, size_t old_size, size_t new_size);
static size_t next_size(size_t old_size, size_t additional_chars, size_t old_allocated_size);
static size_t count_digits(size_t number);
void expand_if_needed(char **memcell, size_t *current_allocated_size, size_t additional_chars, size_t used_size);

char *rle_encode(const char *input_buffer, size_t input_size, size_t *outsize) {
    assert(input_buffer != NULL);
    assert(input_size   != NULL);
    assert(outsize      != NULL);

    *outsize = 0;
    size_t current_allocated_size = input_size * 2;
    char *encoded = (char *)calloc(current_allocated_size, sizeof(char));

    for(size_t i = 0; i < input_size; ) {
        //counting same characters
        size_t counter = 0;
        while(i + counter < input_size && input_buffer[i + counter] == input_buffer[i])
            counter++;

        //expanding allocated memory if needed
        size_t printing_size = count_digits(counter) + 1;
        expand_if_needed(&encoded, &current_allocated_size, printing_size, *outsize);
        if(encoded == NULL)
            return NULL;

        //printing to encoded string in form nc, where n is number and c is letter
        *outsize += sprintf(encoded + *outsize, "%llu%c", counter, input_buffer[i]);
        i += counter;
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
    if(buffer == NULL)
        return NULL;

    if(fread(buffer, sizeof(char), *outsize, input) != *outsize) {
        free(buffer);
        buffer = NULL;
        return NULL;
    }
    for(size_t i =0; i < *outsize; i++)
        if(!isalpha(buffer[i])) {
            free(buffer);
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
    size_t current_allocated_size = 2 * encoded_size;
    char *decoded = (char *)calloc(current_allocated_size, sizeof(char));
    if(decoded == NULL)
        return NULL;

    for(size_t i = 0; i < encoded_size; ) {
        //getting number for one character and this particular character
        size_t counter = 0;
        for( ; isdigit(encoded[i]); i++)
            counter = 10 * counter + (encoded[i] - '0');
        char c = encoded[i++];

        //expanding allocated memory if needed
        expand_if_needed(&decoded, &current_allocated_size, counter, *outsize);
        if(decoded == NULL)
            return NULL;

        //printing out charracters
        memset(decoded + *outsize, c, counter);
        *outsize += counter;
    }
    return decoded;
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

//finds next size depending on current size and number of characters needed to add
size_t next_size(size_t old_size, size_t additional_chars, size_t old_allocated_size) {
    size_t new_size = old_allocated_size * 2;
    while(old_size + additional_chars > new_size / 4)
        new_size *= 2;

    return new_size;
}

//counts digits in number
size_t count_digits(size_t number) {
    size_t digits_number = 0;
    while(number != 0) {
        digits_number++;
        number /= 10;
    }
    return digits_number;
}

//expands memory cell if current used size together with needed additional reaches half of the size
void expand_if_needed(char **memcell, size_t *current_allocated_size, size_t additional_chars, size_t used_size) {
    if(used_size + additional_chars < (*current_allocated_size) / 2)
        return ;

    size_t new_size = next_size(used_size, additional_chars, *current_allocated_size);
    char *new_memcell = (char *)recalloc(*memcell, *current_allocated_size, new_size);
    *memcell = new_memcell;
    *current_allocated_size = new_size;
}
