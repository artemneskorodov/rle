#ifndef RLE_H
#define RLE_H

#include <stdio.h>
#include <stdlib.h>

enum coding_state_t {
    CODING_SUCCESS,
    CODING_ERROR,
    READING_ERROR
};

const char *const DEFAULT_FILE_NAME = "input.txt";

//encodes input buffer in read length encoding (don't forget to free result)
char *rle_encode(const char *input_buffer, size_t input_size, size_t *outsize);

//decodes input buffer in read length encoding (don't forget to free result)
char *rle_decode(const char *encoded, size_t encoded_size, size_t *outsize);

#endif
