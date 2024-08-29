#ifndef HANDLERS_H
#define HANDLERS_H

#include "handle_flags.h"

exit_code_t handle_help(const int argc, const char *argv[]);
exit_code_t handle_encode(const int argc, const char *argv[]);
exit_code_t handle_decode(const int argc, const char *argv[]);
exit_code_t handle_test(const int argc, const char *argv[]);

#endif
