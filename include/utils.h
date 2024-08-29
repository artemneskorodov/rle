#ifndef UTILS_H
#define UTILS_H

char *read_file(const char *filename, size_t *outsize);
char *copy_string(const char *string, size_t size);
void *recalloc(void *memcell, size_t old_size, size_t new_size);
long file_size(FILE *file);
size_t get_input(char *buffer, size_t max_size);
size_t count_digits(size_t number);
void clean_console(void);

#endif
