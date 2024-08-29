#ifndef HANDLE_FLAGS_H
#define HANDLE_FLAGS_H

enum flags_state_t {
    FLAGS_ERROR,
    FLAGS_SUCCESS
};

enum exit_code_t {
    EXIT_CODE_SUCCESS,
    EXIT_CODE_FAILURE
};

struct program_mode_t {
    const char *long_name;
    const char *short_name;
    exit_code_t (*handler)(const int, const char *[]);
};

const size_t MAX_MODES_NUMBER = 64;

struct program_config_t {
    program_mode_t modes[MAX_MODES_NUMBER];
    size_t modes_number;
    exit_code_t (*deafault_handler)(const int, const char *[]);
};

flags_state_t register_mode(program_config_t *config, const char *long_name,
                            const char *short_name, exit_code_t (*handler)(const int, const char *[]));
flags_state_t choose_default(program_config_t *config, exit_code_t (*handler)(const int, const char *[]));

exit_code_t parse_flags(const int argc, const char *argv[], program_config_t *config);

exit_code_t handle_unknown_flag(const char *flag);

#endif
