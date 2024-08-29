#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "rle.h"
#include "utils.h"

static size_t next_size(size_t old_size, size_t additional_chars, size_t old_allocated_size);
static size_t count_digits(size_t number);
void expand_if_needed(char **memcell, size_t *current_allocated_size, size_t additional_chars, size_t used_size);

//ENCODING
char *rle_encode(const char *input_buffer, size_t input_size, size_t *outsize) {
    assert(input_buffer != NULL);
    assert(input_size   != NULL);
    assert(outsize      != NULL);

    for(size_t i = 0; i < input_size; i++)
        if(!isalpha(input_buffer[i]))
            return NULL;

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

//DECODING
char *rle_decode(const char *encoded, size_t encoded_size, size_t *outsize) {
    assert(encoded != NULL);
    assert(outsize != NULL);

    for(size_t i = 0; i < encoded_size; i++)
        if(!isalpha(encoded[i]) && !isdigit(encoded[i]))
            return NULL;

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
