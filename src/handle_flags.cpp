#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "handle_flags.h"

exit_code_t handle_unknown_flag(const char *flag) {
    printf("Unknown_flag %s\n", flag);
    return EXIT_CODE_FAILURE;
}

flags_state_t register_mode(program_config_t *config, const char *long_name,
                            const char *short_name, exit_code_t (*handler)(const int, const char *[])) {

    assert(config     != NULL);
    assert(long_name  != NULL);
    assert(short_name != NULL);
    assert(handler    != NULL);

    if(config->modes_number == MAX_MODES_NUMBER)
        return FLAGS_ERROR;

    config->modes[config->modes_number].long_name  = long_name ;
    config->modes[config->modes_number].short_name = short_name;
    config->modes[config->modes_number].handler    = handler   ;

    config->modes_number += 1;

    return FLAGS_SUCCESS;
}
flags_state_t choose_default(program_config_t *config, exit_code_t (*handler)(const int, const char *[])) {
    assert(config  != NULL);
    assert(handler != NULL);

    config->deafault_handler = handler;
    return FLAGS_SUCCESS;
}

exit_code_t parse_flags(const int argc, const char *argv[], program_config_t *config) {
    assert(argv   != NULL);
    assert(config != NULL);
    assert(argc   != 0   );

    while(config->modes_number != 0 &&
          strcmp(config->modes[config->modes_number - 1].long_name , argv[1]) != 0 &&
          strcmp(config->modes[config->modes_number - 1].short_name, argv[1]) != 0)
        config->modes_number -= 1;

    if(config->modes_number == 0)
        return handle_unknown_flag(argv[1]);

    return config->modes[config->modes_number - 1].handler(argc, argv);
}
