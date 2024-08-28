#include "rle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *get_filename(int argc, const char *argv[]);

int main(int argc, const char *argv[]) {
    const char *filename = get_filename(argc, argv);
    if(filename == NULL)
        return EXIT_FAILURE;

    FILE *input_file = fopen(filename, "r");
    if(input_file == NULL) {
        printf("There is no file: %s\n", filename);
        return EXIT_FAILURE;
    }

    size_t input_size = 0;
    char *input = read_file(input_file, &input_size);
    if(input == NULL) {
        fclose(input_file);
        printf("Error reading input file\n");
        return EXIT_FAILURE;
    }
    fclose(input_file);

    size_t encoded_size = 0;
    char *encoded = rle_encode(input, input_size, &encoded_size);
    if(encoded == NULL) {
        printf("Error, while encoding message\n");
        free(input);
        return EXIT_FAILURE;
    }

    double compression_ratio = (double)input_size / (double)encoded_size;

    size_t decoded_size = 0;
    char *decoded = rle_decode(encoded, encoded_size, &decoded_size);
    if(decoded == NULL) {
        printf("Error, while decoding message\n");
        free(input);
        free(encoded);
        return EXIT_FAILURE;
    }

    printf("\n"
           "|   Original: %s \n"
           "|    Encoded: %s \n"
           "|Compression: %lg\n"
           "|    Decoded: %s \n\n",
           input, encoded, compression_ratio, decoded);

    free(encoded);
    free(input);
    free(decoded);
    return EXIT_SUCCESS;
}

const char *get_filename(int argc, const char *argv[]) {
    if(argc == 1) {
        return DEFAULT_FILE_NAME;
    }
    else if(argc == 2) {
        return argv[1];
    }
    else{
        printf("Unexpected parametre: %s\n", argv[2]);
        return NULL;
    }
}
