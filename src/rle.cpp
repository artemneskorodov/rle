#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "rle.h"
#include "utils.h"

static size_t next_size(size_t old_size, size_t additional_chars, size_t old_allocated_size);
static void expand_if_needed(char **memcell, size_t *current_allocated_size, size_t additional_chars, size_t used_size);

//ENCODING
char *rle_encode(const char *decoded, size_t decoded_size, size_t *outsize) {
    assert(decoded != NULL);
    assert(outsize != NULL);

    for(size_t i = 0; i < decoded_size; i++)
        if(isdigit(decoded[i]))
            return NULL;

    *outsize = 0;
    size_t current_allocated_size = decoded_size * 2;
    char *encoded = (char *)calloc(current_allocated_size, sizeof(char));

    for(size_t index = 0; index < decoded_size; ) {
        //counting same characters
        size_t counter = 0;
        while(index + counter < decoded_size && decoded[index + counter] == decoded[index])
            counter++;

        //expanding allocated memory if needed
        size_t printing_size = count_digits(counter) + 1;
        expand_if_needed(&encoded, &current_allocated_size, printing_size, *outsize);
        if(encoded == NULL)
            return NULL;

        //printing to encoded string in form nc, where n is number and c is letter
        *outsize += sprintf(encoded + *outsize, "%llu%c", counter, decoded[index]);
        index += counter;
    }
    return encoded;
}

//DECODING
char *rle_decode(const char *encoded, size_t encoded_size, size_t *outsize) {
    assert(encoded != NULL);
    assert(outsize != NULL);

    *outsize = 0;
    size_t current_allocated_size = 2 * encoded_size;
    char *decoded = (char *)calloc(current_allocated_size, sizeof(char));
    if(decoded == NULL)
        return NULL;

    for(size_t index = 0; index < encoded_size; ) {
        //getting number for one character and this particular character
        size_t counter = 0;
        while(!isdigit(encoded[index]) && index < encoded_size)
            index++;

        for( ; isdigit(encoded[index]); index++)
            counter = 10 * counter + (encoded[index] - '0');
        char c = encoded[index++];

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

//finds next size depending on current size and number of characters needed to add
size_t next_size(size_t old_size, size_t additional_size, size_t allocated_memory) {
    size_t new_size = allocated_memory * 2;
    while(old_size + additional_size > new_size / 4)
        new_size *= 2;

    return new_size;
}

//expands memory cell if current used size together with needed additional reaches half of the size
void expand_if_needed(char **memcell, size_t *current_allocated_size, size_t additional_chars, size_t used_size) {
    assert(current_allocated_size != NULL);
    assert(memcell                != NULL);

    if(used_size + additional_chars >= (*current_allocated_size) / 2) {
        size_t new_size = next_size(used_size, additional_chars, *current_allocated_size);
        char *new_memcell = (char *)recalloc(*memcell, *current_allocated_size, new_size);

        *memcell = new_memcell;
        *current_allocated_size = new_size;
    }
}
