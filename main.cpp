#include <stdio.h>
#include <stdlib.h>
#include "handle_flags.h"
#include "utils.h"
#include "handlers.h"
int main(const int argc, const char *argv[]) {
    program_config_t config = {};
    if(register_mode (&config, "--help"  , "-h", handle_help  ) != FLAGS_SUCCESS ||
       register_mode (&config, "--encode", "-e", handle_encode) != FLAGS_SUCCESS ||
       register_mode (&config, "--decode", "-d", handle_decode) != FLAGS_SUCCESS ||
       register_mode (&config, "--test"  , "-t", handle_test  ) != FLAGS_SUCCESS ||
       choose_default(&config,                   handle_encode) != FLAGS_SUCCESS)
        return EXIT_FAILURE;
    return (int)parse_flags(argc, argv, &config);
}
