#ifndef LOGGER_H
#define LOGGER_H

#define CPRINT_RED   "\033[31m"
#define CPRINT_GREEN "\033[32m"
#define CPRINT_STOP  "\033[0m"

#include "herr.h"

err_t logger_use_color(const char *str);

void cprint(const char *color, const char *fmt, ...);

void log_debug(const char *fmt, ...);

#endif
