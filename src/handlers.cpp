#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "handle_flags.h"
#include "rle.h"
#include "utils.h"
#include "handlers.h"

static const size_t MAX_USER_INPUT = 10;

static exit_code_t handle_encode_from_console(void);
static exit_code_t handle_encode_from_file(const char *filename);
static exit_code_t handle_decode_from_console(void);
static exit_code_t handle_decode_from_file(const char *filename);
static exit_code_t check_encoding(const char *encoded, const char *decoded, size_t decoded_size);
static exit_code_t check_decoding(const char *encoded, const char *decoded, size_t encoded_size);

exit_code_t handle_help(const int argc, const char *argv[]) {
    if(argc > 2)
        return handle_unknown_flag(argv[2]);
    printf("'--help'   or '-h' for help\n"
           "'--encode' or '-e' to encode (type file name to read from file)\n"
           "'--decode' or '-d' to decode (type file name to read from file)\n"
           "'--test'   or '-t' to run tests "
           "(first parameter has to be file with decoded text and second expected encoded)\n");
    return EXIT_CODE_SUCCESS;
}

exit_code_t handle_encode(const int argc, const char *argv[]) {
    assert(argc >= 2   );
    assert(argv != NULL);
    if(argc == 2)
        return handle_encode_from_console();
    else if(argc == 3)
        return handle_encode_from_file(argv[2]);
    else
        return handle_unknown_flag(argv[3]);
}

exit_code_t handle_decode(const int argc, const char *argv[]) {
    assert(argc >= 2   );
    assert(argv != NULL);
    if(argc == 2)
        return handle_decode_from_console();
    else if(argc == 3)
        return handle_decode_from_file(argv[2]);
    else
        return handle_unknown_flag(argv[3]);
}

exit_code_t handle_test(const int argc, const char *argv[]) {
    assert(argc >= 2   );
    assert(argv != NULL);

    if(argc < 4) {
        printf("Enter file names('--help' to get helped)\n");
        return EXIT_CODE_FAILURE;
    }
    if(argc > 4) {
        handle_unknown_flag(argv[4]);
        return EXIT_CODE_FAILURE;
    }

    size_t encoded_size = 0, decoded_size = 0;
    char *encoded = NULL;
    char *decoded = NULL;
    if((encoded = read_file(argv[2], &encoded_size)) == NULL) {
        printf("Error while reading from %s\n", argv[2]);
        return EXIT_CODE_FAILURE;
    }
    if((decoded = read_file(argv[3], &decoded_size)) == NULL) {
        printf("Error while reading from %s\n", argv[3]);
        free(encoded);
        return EXIT_CODE_FAILURE;
    }

    if(check_encoding(encoded, decoded, decoded_size) != EXIT_CODE_SUCCESS) {
        free(encoded);
        free(decoded);
        return EXIT_CODE_FAILURE;
    }

    if(check_decoding(encoded, decoded, encoded_size) != EXIT_CODE_SUCCESS) {
        free(encoded);
        free(decoded);
        return EXIT_CODE_FAILURE;
    }


    free(encoded);
    free(decoded);
    printf("Test were successful\n");
    return EXIT_CODE_SUCCESS;
}

exit_code_t handle_encode_from_console(void) {
    char input[MAX_USER_INPUT + 1] = {};
    printf("Enter string to encode:\n");
    size_t input_size = get_input(input, MAX_USER_INPUT);

    size_t encoded_size = 0;
    char *encoded = rle_encode(input, input_size, &encoded_size);

    if(encoded == NULL) {
        printf("Error, while encoding:\n%s\n", input);
        return EXIT_CODE_FAILURE;
    }

    printf("Encoded with rle:\n%s\nCompression: %f\n", encoded, (double)input_size / (double)encoded_size);
    free(encoded);

    return EXIT_CODE_SUCCESS;
}

exit_code_t handle_encode_from_file(const char *filename) {
    assert(filename != NULL);

    size_t input_size;
    char *input = read_file(filename, &input_size);
    if(input == NULL) {
        printf("Error, while reading file %s\n"
               "Check if name is written correctly and\n"
               "file contains only alpha characters (no spaces and new line too)",
               filename);
        return EXIT_CODE_FAILURE;
    }

    size_t encoded_size = 0;
    char *encoded = rle_encode(input, input_size, &encoded_size);

    if(encoded == NULL) {
        printf("Error, while encoding:\n%s\n", input);
        return EXIT_CODE_FAILURE;
    }

    printf("Encoded with rle:\n%s\nCompression: %f\n", encoded, (double)input_size / (double)encoded_size);
    free(encoded);

    return EXIT_CODE_SUCCESS;
}

exit_code_t handle_decode_from_console(void) {
    char input[MAX_USER_INPUT + 1] = {};
    printf("Enter string to decode:\n");
    size_t input_size = get_input(input, MAX_USER_INPUT);

    size_t decoded_size = 0;
    char *decoded = rle_decode(input, input_size, &decoded_size);

    if(decoded == NULL) {
        printf("Error, while decoding:\n%s\n", input);
        return EXIT_CODE_FAILURE;
    }

    printf("Encoded with rle:\n%s\nCompression: %f\n", decoded, (double)decoded_size / (double)input_size);
    free(decoded);

    return EXIT_CODE_SUCCESS;
}

exit_code_t handle_decode_from_file(const char *filename) {
    assert(filename != NULL);

    size_t input_size;
    char *input = read_file(filename, &input_size);
    if(input == NULL) {
        printf("Error, while reading file %s\n"
               "Check if name is written correctly\n",
               filename);
        return EXIT_CODE_FAILURE;
    }

    size_t decoded_size = 0;
    char *decoded = rle_decode(input, input_size, &decoded_size);

    if(decoded == NULL) {
        printf("Error, while encoding:\n%s\n", input);
        return EXIT_CODE_FAILURE;
    }

    printf("Encoded with rle:\n%s\nCompression: %f\n", decoded, (double)input_size / (double)decoded_size);
    free(decoded);

    return EXIT_CODE_SUCCESS;
}

exit_code_t check_encoding(const char *encoded, const char *decoded, size_t decoded_size) {
    size_t test_size = 0;
    char *test = rle_encode(decoded, decoded_size, &test_size);
    if(test == NULL) {
        printf("Error while encoding\n");
        return EXIT_CODE_FAILURE;
    }

    if(strcmp(test, encoded) != 0) {
        printf("Caught error while encoding:\n%s\n\n"
               "Expected:\n\n%s\n\n"
               "Actual:\n\n%s\n\n", decoded, encoded, test);
        free(test);
        return EXIT_CODE_FAILURE;
    }

    free(test);
    return EXIT_CODE_SUCCESS;
}

exit_code_t check_decoding(const char *encoded, const char *decoded, size_t encoded_size) {
    size_t test_size = 0;
    char *test = rle_decode(encoded, encoded_size, &test_size);
    if(test == NULL) {
        printf("Error while encoding\n");
        return EXIT_CODE_FAILURE;
    }

    if(strcmp(test, decoded) != 0) {
        printf("Caught error while decoding:\n%s\n\n"
               "Expected:\n\n%s\n\n"
               "Actual:\n\n%s\n\n", encoded, decoded, test);
        free(test);
        return EXIT_CODE_FAILURE;
    }

    free(test);
    return EXIT_CODE_SUCCESS;
}
